#include "animation_system.hpp"
#include "frame_pacer.hpp"
#include "resource_manager.hpp"
#include "software_renderer.hpp"
#include "window_manager.hpp"

#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace {

using namespace independent_fighter;

#define CHECK(...) \
    do { \
        if (!(__VA_ARGS__)) { \
            throw std::runtime_error("check failed: " #__VA_ARGS__); \
        } \
    } while (false)

struct TemporaryFile {
    std::filesystem::path path;

    ~TemporaryFile()
    {
        std::error_code error;
        std::filesystem::remove(path, error);
    }
};

TemporaryFile write_test_ppm()
{
    const auto path = std::filesystem::temp_directory_path()
        / "independent_fighter_phase14_sprite.ppm";
    std::ofstream output(path, std::ios::binary);
    if (!output) {
        throw std::runtime_error("could not create temporary PPM");
    }
    output << "P6\n2 1\n255\n";
    constexpr std::array<unsigned char, 6> pixels{255U, 0U, 0U, 0U, 255U, 0U};
    output.write(reinterpret_cast<const char*>(pixels.data()),
        static_cast<std::streamsize>(pixels.size()));
    output.close();
    return TemporaryFile{path};
}

TemporaryFile write_test_animation()
{
    const auto path = std::filesystem::temp_directory_path()
        / "independent_fighter_phase14_walk.anim";
    std::ofstream output(path);
    if (!output) {
        throw std::runtime_error("could not create temporary animation definition");
    }
    output << "# Test-only animation; no original frames\n"
           << "loop false\n"
           << "frame 0 0 8 8 2\n"
           << "frame 8 0 8 8 3\n";
    return TemporaryFile{path};
}

void test_ppm_loading_caching_and_release()
{
    const TemporaryFile file = write_test_ppm();
    ResourceManager resources{};
    const TextureHandle handle = resources.load_ppm(file.path);
    CHECK(handle);
    CHECK(resources.texture_count() == 1U);
    CHECK(resources.texture(handle) != nullptr);
    CHECK(resources.texture(handle)->width == 2);
    CHECK(resources.texture(handle)->height == 1);
    CHECK(resources.texture(handle)->pixel(0, 0) == Color{255U, 0U, 0U, 255U});
    CHECK(resources.texture(handle)->pixel(1, 0) == Color{0U, 255U, 0U, 255U});
    CHECK(resources.load_ppm(file.path) == handle);
    CHECK(resources.texture_count() == 1U);
    CHECK(resources.unload_texture(handle));
    CHECK(resources.texture(handle) == nullptr);
    CHECK(resources.texture_count() == 0U);
}

void test_checker_placeholder()
{
    ResourceManager resources{};
    const TextureHandle handle = resources.create_placeholder(
        "test-placeholder", 16, 8, Color{255U, 0U, 255U, 255U}, Color{0U, 0U, 0U, 255U});
    const Texture* texture = resources.texture(handle);
    CHECK(texture != nullptr);
    CHECK(texture->pixel(0, 0) == Color{255U, 0U, 255U, 255U});
    CHECK(texture->pixel(8, 0) == Color{0U, 0U, 0U, 255U});
}

void test_animation_load_switch_duration_and_looping()
{
    const TemporaryFile file = write_test_animation();
    ResourceManager resources{};
    const AnimationClip& loaded = resources.load_animation_definition(file.path);
    CHECK(loaded.name == "independent_fighter_phase14_walk");
    CHECK(!loaded.looping);
    CHECK(loaded.frames.size() == 2U);
    CHECK(loaded.frames[0].duration_ticks == 2U);

    std::unordered_map<MovementState, AnimationClip> clips{};
    clips.emplace(MovementState::Walking, loaded);
    clips.emplace(MovementState::Standing, AnimationClip{
        "idle", {AnimationFrame{RectI{0, 0, 8, 8}, 2U}}, true
    });

    AnimationController controller{};
    CHECK(controller.sync_movement(MovementState::Walking, clips));
    CHECK(controller.current_name() == loaded.name);
    CHECK(controller.frame_index() == 0U);
    controller.advance_ticks(1U);
    CHECK(controller.frame_index() == 0U);
    controller.advance_ticks(1U);
    CHECK(controller.frame_index() == 1U);
    controller.advance_ticks(3U);
    CHECK(controller.finished());
    CHECK(controller.frame_index() == 1U);

    CHECK(controller.sync_movement(MovementState::Standing, clips));
    CHECK(controller.current_name() == "idle");
    CHECK(!controller.finished());
    controller.advance_ticks(2U);
    CHECK(controller.frame_index() == 0U);
    CHECK(!controller.finished());
    CHECK(!controller.sync_movement(MovementState::Standing, clips));

    const AnimationClip special{
        "special", {AnimationFrame{RectI{16, 0, 8, 8}, 1U}}, false
    };
    controller.play_clip(special);
    CHECK(controller.current_name() == "special");
    CHECK(controller.frame_index() == 0U);

    CHECK(resources.animation_count() == 1U);
    CHECK(resources.unload_animation(loaded.name));
    CHECK(resources.animation_count() == 0U);
}

void test_layered_sprite_rendering_and_background()
{
    ResourceManager resources{};
    const TextureHandle background = resources.create_placeholder(
        "background", 1, 1, Color{0U, 0U, 255U, 255U}, Color{0U, 0U, 255U, 255U});
    const TextureHandle character = resources.create_placeholder(
        "character", 1, 1, Color{255U, 0U, 0U, 255U}, Color{255U, 0U, 0U, 255U});
    const TextureHandle ui = resources.create_placeholder(
        "ui", 1, 1, Color{0U, 255U, 0U, 255U}, Color{0U, 255U, 0U, 255U});
    SoftwareRenderer renderer{4, 4};
    const std::array<SpriteDrawCommand, 3> commands{{
        SpriteDrawCommand{ui, RectI{0, 0, 1, 1}, RectI{1, 1, 1, 1},
            RenderLayer::UserInterface, 0, false},
        SpriteDrawCommand{character, RectI{0, 0, 1, 1}, RectI{1, 1, 2, 2},
            RenderLayer::Characters, 0, false},
        SpriteDrawCommand{background, RectI{0, 0, 1, 1}, RectI{0, 0, 4, 4},
            RenderLayer::Background, 0, false}
    }};

    renderer.render(resources, Color{10U, 10U, 10U, 255U}, commands);
    CHECK(renderer.frame().pixel(0, 0) == Color{0U, 0U, 255U, 255U});
    CHECK(renderer.frame().pixel(1, 1) == Color{0U, 255U, 0U, 255U});
    CHECK(renderer.frame().pixel(2, 1) == Color{255U, 0U, 0U, 255U});
}

void test_alpha_blending_and_horizontal_flip()
{
    const TemporaryFile file = write_test_ppm();
    ResourceManager resources{};
    const TextureHandle two_colors = resources.load_ppm(file.path);
    const TextureHandle half_blue = resources.create_placeholder(
        "half-blue", 1, 1, Color{0U, 0U, 255U, 128U}, Color{0U, 0U, 255U, 128U});
    SoftwareRenderer renderer{2, 1};
    const std::array<SpriteDrawCommand, 2> commands{{
        SpriteDrawCommand{two_colors, RectI{0, 0, 2, 1}, RectI{0, 0, 2, 1},
            RenderLayer::Background, 0, true},
        SpriteDrawCommand{half_blue, RectI{0, 0, 1, 1}, RectI{0, 0, 1, 1},
            RenderLayer::Effects, 0, false}
    }};

    renderer.render(resources, Color{0U, 0U, 0U, 255U}, commands);
    CHECK(renderer.frame().pixel(0, 0).g == 127U);
    CHECK(renderer.frame().pixel(0, 0).b == 128U);
    CHECK(renderer.frame().pixel(1, 0) == Color{255U, 0U, 0U, 255U});
}

void test_viewport_scaling_resize_and_close()
{
    WindowManager window{};
    CHECK(window.open(WindowConfig{"test", 2, 2, 8, 6, true}));
    CHECK(window.is_open());
    CHECK(window.viewport().scale == 3.0);
    CHECK(window.viewport().x == 1);
    CHECK(window.viewport().y == 0);
    CHECK(window.viewport().width == 6);
    CHECK(window.viewport().height == 6);

    FrameBuffer logical{};
    logical.resize(2, 2);
    logical.pixel(0, 0) = Color{255U, 0U, 0U, 255U};
    logical.pixel(1, 0) = Color{0U, 255U, 0U, 255U};
    logical.pixel(0, 1) = Color{0U, 0U, 255U, 255U};
    logical.pixel(1, 1) = Color{255U, 255U, 255U, 255U};
    CHECK(window.present(logical));
    CHECK(window.output_frame().pixel(0, 0) == Color{0U, 0U, 0U, 255U});
    CHECK(window.output_frame().pixel(1, 0) == Color{255U, 0U, 0U, 255U});
    CHECK(window.output_frame().pixel(7, 5) == Color{0U, 0U, 0U, 255U});

    CHECK(window.resize_output(5, 5));
    CHECK(window.viewport().width == 4);
    CHECK(window.viewport().height == 4);
    CHECK(!window.present(FrameBuffer{}));
    window.close();
    CHECK(!window.is_open());
}

void test_frame_pacing()
{
    const FramePacer::TimePoint start{};
    FramePacer pacer{60U};
    const FramePacer::TimePoint first = pacer.schedule_next(start);
    CHECK(first > start);
    CHECK(pacer.time_until_next(start) == first - start);
    const FramePacer::TimePoint second = pacer.schedule_next(first);
    CHECK(second > first);
    const FramePacer::TimePoint late = second + std::chrono::milliseconds(5);
    const FramePacer::TimePoint recovered = pacer.schedule_next(late);
    CHECK(recovered == late + (first - start));
    CHECK(pacer.refresh_hz() == 60U);
    pacer.reset();
    CHECK(pacer.time_until_next(start) == FramePacer::Duration::zero());

    bool rejected_zero_rate = false;
    try {
        const FramePacer invalid{0U};
        (void)invalid;
    } catch (const std::invalid_argument&) {
        rejected_zero_rate = true;
    }
    CHECK(rejected_zero_rate);
}

struct TestCase {
    std::string_view name;
    void (*run)();
};

} // namespace

int main()
{
    constexpr TestCase tests[]{
        {"PPM loading, caching, and release", test_ppm_loading_caching_and_release},
        {"checker placeholder", test_checker_placeholder},
        {"animation transitions and timing", test_animation_load_switch_duration_and_looping},
        {"layered rendering and background", test_layered_sprite_rendering_and_background},
        {"alpha blending and sprite flip", test_alpha_blending_and_horizontal_flip},
        {"viewport scaling and lifecycle", test_viewport_scaling_resize_and_close},
        {"frame pacing", test_frame_pacing}
    };

    int passed = 0;
    for (const TestCase& test : tests) {
        try {
            test.run();
            ++passed;
            std::cout << "PASS " << test.name << '\n';
        } catch (const std::exception& error) {
            std::cerr << "FAIL " << test.name << ": " << error.what() << '\n';
            return 1;
        }
    }

    std::cout << passed << " graphics tests passed\n";
    return 0;
}
