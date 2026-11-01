#pragma once

#include "audio_engine.hpp"
#include "combat_system.hpp"
#include "game_loop.hpp"
#include "input_manager.hpp"
#include "software_renderer.hpp"
#include "window_manager.hpp"

#include <memory>
#include <span>
#include <string>
#include <array>
#include <unordered_map>

namespace independent_fighter {

struct ApplicationConfig {
    WindowConfig window{};
    AudioConfig audio{};
    CombatConfig combat{};
    bool enable_graphics{true};
    bool enable_audio{true};
};

struct RuntimeTickResult {
    bool success{};
    std::uint32_t simulation_steps{};
    bool rendered{};
    std::string error{};
};

class GameApplication {
public:
    explicit GameApplication(
        ApplicationConfig config = {},
        std::unique_ptr<AudioOutput> audio_output = {});
    ~GameApplication();

    GameApplication(const GameApplication&) = delete;
    GameApplication& operator=(const GameApplication&) = delete;

    bool initialize() noexcept;
    void shutdown() noexcept;
    RuntimeTickResult tick(
        double elapsed_seconds,
        std::span<const InputEvent> input_events = {}) noexcept;

    bool initialized() const noexcept;
    const std::string& last_error() const noexcept;
    GameState state() const noexcept;
    const PlayerState& player(PlayerId id) const noexcept;
    const FrameBuffer* output_frame() const noexcept;

    InputManager& input_manager() noexcept;
    GameSession& game_session() noexcept;
    CombatSystem& combat_system() noexcept;
    AudioEngine& audio_engine() noexcept;

private:
    bool prepare_graphics();
    void advance_simulation_tick();
    bool render_frame();
    void consume_audio_events();
    void consume_frame_actions() noexcept;
    static std::size_t player_index(PlayerId id) noexcept;

    ApplicationConfig config_{};
    GameLoop game_loop_{};
    GameSession game_session_{};
    InputManager input_manager_{};
    CombatSystem combat_system_{};
    AudioEngine audio_engine_;
    ResourceManager graphics_resources_{};
    WindowManager window_{};
    std::unique_ptr<SoftwareRenderer> renderer_{};
    std::array<TextureHandle, 2> character_textures_{};
    TextureHandle stage_texture_{};
    std::unordered_map<MovementState, AnimationClip> movement_clips_{};
    std::array<AnimationController, 2> animations_{};
    std::uint64_t simulation_tick_{};
    bool initialized_{};
    std::string last_error_{};
};

} // namespace independent_fighter
