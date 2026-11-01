#include "game_application.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <exception>
#include <stdexcept>
#include <utility>
#include <vector>

namespace independent_fighter {
namespace {

AnimationClip make_movement_clip(const char* name, int first_x, std::uint32_t duration)
{
    return AnimationClip{
        name,
        {
            AnimationFrame{RectI{first_x, 0, 16, 24}, duration},
            AnimationFrame{RectI{first_x + 16, 0, 16, 24}, duration}
        },
        true
    };
}

AnimationClip make_reaction_clip()
{
    return AnimationClip{
        "hit-reaction",
        {
            AnimationFrame{RectI{64, 0, 16, 24}, 3U},
            AnimationFrame{RectI{80, 0, 16, 24}, 3U}
        },
        false
    };
}

} // namespace

GameApplication::GameApplication(
    ApplicationConfig config,
    std::unique_ptr<AudioOutput> audio_output)
    : config_(std::move(config)),
      combat_system_(config_.combat),
      audio_engine_(config_.audio, std::move(audio_output))
{
}

GameApplication::~GameApplication()
{
    shutdown();
}

bool GameApplication::initialize() noexcept
{
    if (initialized_) {
        return true;
    }
    try {
        if (!combat_system_.valid()) {
            last_error_ = "initialization failed: invalid combat configuration";
            return false;
        }
        game_loop_.reset();
        simulation_tick_ = 0U;
        combat_system_.reset();
        input_manager_.clear_runtime_state();
        game_session_.initialize();
        if (config_.enable_graphics && !prepare_graphics()) {
            last_error_ = "graphics initialization failed: invalid window configuration";
            shutdown();
            return false;
        }
        if (config_.enable_audio && !audio_engine_.initialize()) {
            last_error_ = "audio initialization failed";
            shutdown();
            return false;
        }
        initialized_ = true;
        if (config_.enable_audio) {
            consume_audio_events();
        } else {
            game_session_.drain_events();
        }
        return true;
    } catch (const std::bad_alloc&) {
        last_error_ = "initialization failed: insufficient memory";
    } catch (const std::exception& error) {
        last_error_ = std::string("initialization failed: ") + error.what();
    } catch (...) {
        last_error_ = "initialization failed: unknown error";
    }
    shutdown();
    return false;
}

void GameApplication::shutdown() noexcept
{
    game_loop_.reset();
    simulation_tick_ = 0U;
    combat_system_.reset();
    input_manager_.clear_runtime_state();
    audio_engine_.shutdown();
    window_.close();
    renderer_.reset();
    graphics_resources_.clear();
    character_textures_ = {};
    stage_texture_ = {};
    movement_clips_.clear();
    initialized_ = false;
}

RuntimeTickResult GameApplication::tick(
    double elapsed_seconds,
    std::span<const InputEvent> input_events) noexcept
{
    if (!initialized_) {
        return RuntimeTickResult{false, 0U, false, "application is not initialized"};
    }
    try {
        for (const InputEvent& event : input_events) {
            input_manager_.handle_event(event);
        }

        const FrameAdvance advance = game_loop_.advance(elapsed_seconds);
        for (std::uint32_t step = 0U; step < advance.simulation_steps; ++step) {
            advance_simulation_tick();
        }
        const bool rendered = config_.enable_graphics ? render_frame() : false;
        if (config_.enable_graphics && !rendered) {
            return RuntimeTickResult{false, advance.simulation_steps, false,
                "graphics presentation failed"};
        }
        return RuntimeTickResult{true, advance.simulation_steps, rendered, {}};
    } catch (const std::bad_alloc&) {
        last_error_ = "frame failed: insufficient memory";
    } catch (const std::exception& error) {
        last_error_ = std::string("frame failed: ") + error.what();
    } catch (...) {
        last_error_ = "frame failed: unknown error";
    }
    return RuntimeTickResult{false, 0U, false, last_error_};
}

bool GameApplication::initialized() const noexcept
{
    return initialized_;
}

const std::string& GameApplication::last_error() const noexcept
{
    return last_error_;
}

GameState GameApplication::state() const noexcept
{
    return game_session_.state();
}

const PlayerState& GameApplication::player(PlayerId id) const noexcept
{
    return game_session_.player(id);
}

const FrameBuffer* GameApplication::output_frame() const noexcept
{
    return config_.enable_graphics ? &window_.output_frame() : nullptr;
}

InputManager& GameApplication::input_manager() noexcept
{
    return input_manager_;
}

GameSession& GameApplication::game_session() noexcept
{
    return game_session_;
}

CombatSystem& GameApplication::combat_system() noexcept
{
    return combat_system_;
}

AudioEngine& GameApplication::audio_engine() noexcept
{
    return audio_engine_;
}

bool GameApplication::prepare_graphics()
{
    if (!window_.open(config_.window)) {
        return false;
    }
    renderer_ = std::make_unique<SoftwareRenderer>(
        config_.window.logical_width, config_.window.logical_height);

    stage_texture_ = graphics_resources_.create_placeholder(
        "integration-stage", 64, 36,
        Color{24U, 38U, 72U, 255U}, Color{38U, 58U, 96U, 255U});
    character_textures_[0] = graphics_resources_.create_placeholder(
        "integration-player-one", 96, 24,
        Color{215U, 70U, 74U, 255U}, Color{245U, 205U, 75U, 255U});
    character_textures_[1] = graphics_resources_.create_placeholder(
        "integration-player-two", 96, 24,
        Color{55U, 135U, 220U, 255U}, Color{235U, 235U, 245U, 255U});

    movement_clips_.emplace(MovementState::Standing,
        make_movement_clip("standing", 0, 8U));
    movement_clips_.emplace(MovementState::Walking,
        make_movement_clip("walking", 16, 4U));
    movement_clips_.emplace(MovementState::Running,
        make_movement_clip("running", 32, 3U));
    movement_clips_.emplace(MovementState::Jumping,
        make_movement_clip("jumping", 48, 4U));
    movement_clips_.emplace(MovementState::Falling,
        make_movement_clip("falling", 48, 4U));
    movement_clips_.emplace(MovementState::Crouching,
        make_movement_clip("crouching", 32, 6U));
    movement_clips_.emplace(MovementState::Landing,
        make_movement_clip("landing", 48, 2U));

    if (!window_.resize_output(config_.window.output_width, config_.window.output_height)) {
        return false;
    }
    return true;
}

void GameApplication::advance_simulation_tick()
{
    const FrameInput input = input_manager_.sample_frame(++simulation_tick_, game_session_);
    game_session_.process_frame(input);
    const std::vector<CombatHit> hits = combat_system_.resolve(
        game_session_, input, simulation_tick_);
    for (const CombatHit& hit : hits) {
        game_session_.apply_damage(hit.victim, hit.damage);
    }

    consume_frame_actions();
    for (const PlayerId id : {PlayerId::One, PlayerId::Two}) {
        const PlayerState& state = game_session_.player(id);
        AnimationController& animation = animations_[player_index(id)];
        animation.sync_movement(state.movement, movement_clips_);
        animation.advance_ticks();
    }
    consume_audio_events();
    if (config_.enable_audio
        && !audio_engine_.render_frames(audio_engine_.sample_rate() / 60U)) {
        throw std::runtime_error("audio output rejected simulation block");
    }
}

bool GameApplication::render_frame()
{
    if (renderer_ == nullptr || !window_.is_open()) {
        return false;
    }

    const int logical_width = config_.window.logical_width;
    const int logical_height = config_.window.logical_height;
    std::vector<SpriteDrawCommand> commands;
    commands.reserve(3U);
    commands.push_back(SpriteDrawCommand{
        stage_texture_, RectI{0, 0, 64, 36}, RectI{0, 0, logical_width, logical_height},
        RenderLayer::Background, 0, false
    });

    for (const PlayerId id : {PlayerId::One, PlayerId::Two}) {
        const std::size_t index = player_index(id);
        const PlayerState& player_state = game_session_.player(id);
        const AnimationFrame* frame = animations_[index].current_frame();
        const RectI source = frame == nullptr ? RectI{0, 0, 16, 24} : frame->source;
        const RectI destination{
            static_cast<int>(std::lround(player_state.position.x)) - 16,
            static_cast<int>(std::lround(player_state.position.y)) - 48,
            32,
            48
        };
        commands.push_back(SpriteDrawCommand{
            character_textures_[index], source, destination,
            RenderLayer::Characters, static_cast<int>(index),
            player_state.facing == Facing::Left
        });
    }

    renderer_->render(graphics_resources_, Color{12U, 18U, 38U, 255U}, commands);
    return window_.present(renderer_->frame());
}

void GameApplication::consume_audio_events()
{
    std::vector<GameplayEvent> events = game_session_.drain_events();
    if (events.empty()) {
        return;
    }
    if (config_.enable_audio) {
        audio_engine_.consume_gameplay_events(events);
    }
    for (const GameplayEvent& event : events) {
        if (event.type == GameplayEventType::RoundStarted
            || (event.type == GameplayEventType::StateChanged
                && event.state == GameState::Title)) {
            combat_system_.reset();
        }
        if (event.type == GameplayEventType::HitConfirmed && event.player.has_value()) {
            animations_[player_index(*event.player)].play_clip(make_reaction_clip());
        }
    }
}

void GameApplication::consume_frame_actions() noexcept
{
    for (const PlayerId player_id : {PlayerId::One, PlayerId::Two}) {
        for (const InputAction action : {
                 InputAction::Confirm, InputAction::CycleCharacter,
                 InputAction::Pause, InputAction::Restart,
                 InputAction::LightPunch, InputAction::MediumPunch, InputAction::HeavyPunch,
                 InputAction::LightKick, InputAction::MediumKick, InputAction::HeavyKick}) {
            input_manager_.consume(player_id, action);
        }
    }
}

std::size_t GameApplication::player_index(PlayerId id) noexcept
{
    return id == PlayerId::One ? 0U : 1U;
}

} // namespace independent_fighter
