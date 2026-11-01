#include "game_application.hpp"
#include "character_definition.hpp"
#include "movement_system.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace {

using namespace independent_fighter;

#define CHECK(...) \
    do { \
        if (!(__VA_ARGS__)) { \
            throw std::runtime_error("check failed: " #__VA_ARGS__); \
        } \
    } while (false)

InputEvent key(std::uint32_t code, bool pressed)
{
    return InputEvent{InputEventType::ControlChanged, InputDeviceKind::Keyboard,
        0U, InputControlKind::Key, code, pressed, 0.0F};
}

RuntimeTickResult tick(GameApplication& app, std::initializer_list<InputEvent> events = {})
{
    const std::vector<InputEvent> input(events);
    return app.tick(GameLoop::fixed_step_seconds, input);
}

void start_fight(GameApplication& app)
{
    CHECK(tick(app, {key(default_key::p1_confirm, true)}).success);
    CHECK(app.state() == GameState::CharacterSelect);
    CHECK(tick(app, {
        key(default_key::p1_confirm, false),
        key(default_key::p2_confirm, false)
    }).success);
    CHECK(tick(app, {
        key(default_key::p1_confirm, true),
        key(default_key::p2_confirm, true)
    }).success);
    CHECK(app.state() == GameState::FightIntro);
    CHECK(tick(app, {
        key(default_key::p1_confirm, false),
        key(default_key::p2_confirm, false)
    }).success);
    CHECK(tick(app, {key(default_key::p1_confirm, true)}).success);
    CHECK(app.state() == GameState::Fighting);
}

void enter_fighting(GameSession& session)
{
    FrameInput input{};
    input.confirm = true;
    session.process_frame(input);
    input = {};
    input.player_one.confirm = true;
    input.player_two.confirm = true;
    session.process_frame(input);
    input = {};
    input.confirm = true;
    session.process_frame(input);
    CHECK(session.state() == GameState::Fighting);
}

struct FailingAudioOutput final : AudioOutput {
    bool initialize(std::uint32_t, std::uint16_t) override { return false; }
    void submit(std::span<const std::int16_t>) override {}
    void shutdown() noexcept override { stopped = true; }
    bool stopped{};
};

void test_application_initialization_and_frame_render()
{
    ApplicationConfig config{};
    config.window.logical_width = 160;
    config.window.logical_height = 90;
    config.window.output_width = 320;
    config.window.output_height = 180;
    GameApplication app{config};
    CHECK(app.initialize());
    CHECK(app.initialized());
    CHECK(app.state() == GameState::Title);
    const RuntimeTickResult result = app.tick(GameLoop::fixed_step_seconds);
    CHECK(result.success);
    CHECK(result.rendered);
    CHECK(result.simulation_steps == 1U);
    CHECK(app.output_frame() != nullptr);
    CHECK(app.output_frame()->width == 320);
    CHECK(app.output_frame()->height == 180);
}

void test_input_state_movement_animation_render_audio_integration()
{
    ApplicationConfig config{};
    config.window.logical_width = 320;
    config.window.logical_height = 180;
    config.window.output_width = 320;
    config.window.output_height = 180;
    GameApplication app{config};
    CHECK(app.initialize());
    start_fight(app);

    const double initial_x = app.player(PlayerId::One).position.x;
    const RuntimeTickResult moved = tick(app, {key(input_code::right_arrow, true)});
    CHECK(moved.success);
    CHECK(app.player(PlayerId::One).position.x > initial_x);
    CHECK(moved.rendered);
    CHECK(app.audio_engine().music_playing());

    CHECK(tick(app, {key(input_code::up_arrow, true)}).success);
    CHECK(app.player(PlayerId::One).movement == MovementState::Jumping);
    CHECK(app.audio_engine().active_effect_channels() > 0U);
}

void test_collision_combat_health_and_round_integration()
{
    ApplicationConfig config{};
    config.enable_graphics = false;
    config.enable_audio = false;
    config.combat.heavy_punch_damage = 1200;
    GameApplication app{config};
    CHECK(app.initialize());
    start_fight(app);

    CHECK(tick(app, {key(input_code::right_arrow, true)}).success);
    for (int tick_index = 1; tick_index < 130; ++tick_index) {
        CHECK(tick(app).success);
    }
    CHECK(app.player(PlayerId::One).position.x == 420.0);
    CHECK(tick(app, {key(default_key::p1_heavy_punch, true)}).success);
    CHECK(app.player(PlayerId::Two).health == 0);
    CHECK(app.player(PlayerId::One).rounds_won == 1);
    CHECK(app.state() == GameState::RoundResult);
}

void test_all_attack_strengths_and_cooldown()
{
    GameSession session{};
    enter_fighting(session);
    FrameInput movement{};
    movement.player_one.right = true;
    for (int tick_index = 0; tick_index < 130; ++tick_index) {
        session.process_frame(movement);
    }
    CombatSystem combat{};
    const auto apply_attack = [&](InputAction, bool PlayerInput::*button, std::uint64_t tick_index) {
        FrameInput input{};
        input.player_one.*button = true;
        const std::vector<CombatHit> hits = combat.resolve(session, input, tick_index);
        if (!hits.empty()) {
            session.apply_damage(hits.front().victim, hits.front().damage);
        }
        return hits;
    };

    CHECK(apply_attack(InputAction::LightPunch, &PlayerInput::light_punch, 131U).size() == 1U);
    CHECK(apply_attack(InputAction::MediumPunch, &PlayerInput::medium_punch, 132U).empty());
    CHECK(apply_attack(InputAction::MediumPunch, &PlayerInput::medium_punch, 139U).size() == 1U);
    CHECK(apply_attack(InputAction::HeavyPunch, &PlayerInput::heavy_punch, 147U).size() == 1U);
    CHECK(apply_attack(InputAction::LightKick, &PlayerInput::light_kick, 155U).size() == 1U);
    CHECK(apply_attack(InputAction::MediumKick, &PlayerInput::medium_kick, 163U).size() == 1U);
    CHECK(apply_attack(InputAction::HeavyKick, &PlayerInput::heavy_kick, 171U).size() == 1U);
    CHECK(session.player(PlayerId::Two).health == 490);
    CHECK(session.state() == GameState::Fighting);
}

void test_both_training_character_profiles()
{
    const CharacterDefinition& balanced = character_definition(CharacterId::TrainingBalanced);
    const CharacterDefinition& heavy = character_definition(CharacterId::TrainingHeavy);
    CHECK(balanced.id != heavy.id);
    CHECK(balanced.max_health == 1000);
    CHECK(heavy.max_health == 1100);

    PlayerState balanced_player{};
    PlayerState heavy_player{};
    balanced_player.position = Vec2{100.0, balanced.ground_y};
    heavy_player.position = Vec2{100.0, heavy.ground_y};
    PlayerInput move{};
    move.right = true;
    update_movement(balanced_player, move, balanced);
    update_movement(heavy_player, move, heavy);
    CHECK(balanced_player.position.x == 102.0);
    CHECK(heavy_player.position.x == 101.7);

    move = {};
    move.jump = true;
    update_movement(balanced_player, move, balanced);
    update_movement(heavy_player, move, heavy);
    CHECK(balanced_player.velocity.y != heavy_player.velocity.y);
}

void test_long_session_stability()
{
    ApplicationConfig config{};
    config.enable_graphics = false;
    config.enable_audio = false;
    GameApplication app{config};
    CHECK(app.initialize());
    start_fight(app);

    const auto start = std::chrono::steady_clock::now();
    CHECK(tick(app, {key(input_code::right_arrow, true)}).success);
    std::uint64_t completed_ticks = 1U;
    for (; completed_ticks < 10000U; ++completed_ticks) {
        const RuntimeTickResult result = tick(app);
        CHECK(result.success);
        CHECK(result.simulation_steps == 1U);
    }
    const auto elapsed = std::chrono::steady_clock::now() - start;
    CHECK(app.state() == GameState::Fighting);
    CHECK(app.player(PlayerId::One).position.x == 620.0);
    std::cout << "SOAK ticks=" << completed_ticks << " elapsed_ms="
              << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count()
              << '\n';
}

void test_round_end_victory_and_restart()
{
    GameSession session{};
    enter_fighting(session);
    session.apply_damage(PlayerId::Two, session.player(PlayerId::Two).health);
    FrameInput confirm{};
    confirm.confirm = true;
    session.process_frame(confirm);
    confirm = {};
    confirm.confirm = true;
    session.process_frame(confirm);
    CHECK(session.state() == GameState::Fighting);
    session.apply_damage(PlayerId::Two, session.player(PlayerId::Two).health);
    confirm = {};
    confirm.confirm = true;
    session.process_frame(confirm);
    CHECK(session.state() == GameState::Victory);
    session.restart();
    CHECK(session.state() == GameState::Title);
    CHECK(session.player(PlayerId::One).rounds_won == 0);
    CHECK(session.player(PlayerId::Two).health == 1100);
}

void test_application_shutdown_and_reinitialization()
{
    ApplicationConfig config{};
    config.enable_graphics = false;
    config.enable_audio = false;
    GameApplication app{config};
    CHECK(app.initialize());
    CHECK(app.input_manager().connected_controller_count() == 0U);
    app.input_manager().handle_event(InputEvent{
        InputEventType::DeviceConnected, InputDeviceKind::Gamepad,
        501U, InputControlKind::Button, 0U, false, 0.0F
    });
    CHECK(app.input_manager().is_connected(501U));
    start_fight(app);
    CHECK(tick(app, {key(input_code::right_arrow, true)}).success);
    CHECK(app.player(PlayerId::One).position.x > 160.0);

    app.shutdown();
    CHECK(!app.initialized());
    CHECK(!app.input_manager().is_connected(501U));
    CHECK(app.initialize());
    CHECK(app.state() == GameState::Title);
    CHECK(app.player(PlayerId::One).position.x == 160.0);
    CHECK(!app.input_manager().is_connected(501U));
    CHECK(tick(app).success);
    CHECK(app.state() == GameState::Title);
}

void test_configuration_and_audio_initialization_failures()
{
    ApplicationConfig bad_window{};
    bad_window.window.logical_width = 0;
    GameApplication graphics_failure{bad_window};
    CHECK(!graphics_failure.initialize());
    CHECK(graphics_failure.last_error().find("graphics initialization failed") != std::string::npos);

    ApplicationConfig bad_audio{};
    auto failing_output = std::make_unique<FailingAudioOutput>();
    FailingAudioOutput* output = failing_output.get();
    GameApplication audio_failure{bad_audio, std::move(failing_output)};
    CHECK(!audio_failure.initialize());
    CHECK(audio_failure.last_error().find("audio initialization failed") != std::string::npos);
    CHECK(output->stopped);

    ApplicationConfig bad_combat{};
    bad_combat.combat.attack_reach = 0.0;
    GameApplication combat_failure{bad_combat};
    CHECK(!combat_failure.initialize());
    CHECK(combat_failure.last_error().find("invalid combat configuration") != std::string::npos);
}

struct TestCase {
    std::string_view name;
    void (*run)();
};

} // namespace

int main()
{
    constexpr TestCase tests[]{
        {"application initialize and render", test_application_initialization_and_frame_render},
        {"input/movement/animation/graphics/audio integration", test_input_state_movement_animation_render_audio_integration},
        {"collision/combat/health/round integration", test_collision_combat_health_and_round_integration},
        {"six attack strengths and cooldown", test_all_attack_strengths_and_cooldown},
        {"both training character profiles", test_both_training_character_profiles},
        {"10000-tick session stability", test_long_session_stability},
        {"round result, victory, restart", test_round_end_victory_and_restart},
        {"application shutdown and reinitialization", test_application_shutdown_and_reinitialization},
        {"configuration and audio failure handling", test_configuration_and_audio_initialization_failures}
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

    std::cout << passed << " integration tests passed\n";
    return 0;
}
