#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace independent_fighter {

struct Color {
    std::uint8_t r{};
    std::uint8_t g{};
    std::uint8_t b{};
    std::uint8_t a{255};

    bool operator==(const Color&) const = default;
};

struct RectI {
    int x{};
    int y{};
    int width{};
    int height{};
};

struct TextureHandle {
    std::uint32_t value{};

    explicit operator bool() const noexcept
    {
        return value != 0U;
    }

    bool operator==(const TextureHandle&) const = default;
};

struct Texture {
    int width{};
    int height{};
    std::vector<Color> pixels;

    bool valid() const noexcept;
    const Color& pixel(int x, int y) const;
};

enum class RenderLayer : std::uint8_t {
    Background,
    Stage,
    Characters,
    Effects,
    UserInterface
};

struct SpriteDrawCommand {
    TextureHandle texture{};
    RectI source{};
    RectI destination{};
    RenderLayer layer{RenderLayer::Characters};
    int order{};
    bool flip_horizontal{};
};

struct FrameBuffer {
    int width{};
    int height{};
    std::vector<Color> pixels;

    void resize(int new_width, int new_height);
    void clear(Color color) noexcept;
    const Color& pixel(int x, int y) const;
    Color& pixel(int x, int y);
};

} // namespace independent_fighter
