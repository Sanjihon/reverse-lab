#include "input_manager.hpp"

#include <iostream>
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

InputEvent key_event(std::uint32_t key, bool pressed)
{
    return InputEvent{InputEventType::ControlChanged, InputDeviceKind::Keyboard,
        0U, InputControlKind::Key, key, pressed, 0.0F};
}

InputEvent device_event(InputEventType type, InputDeviceKind kind, InputDeviceId id)
{
    return InputEvent{type, kind, id, InputControlKind::Button, 0U, false, 0.0F};
}

InputEvent button_event(InputDeviceKind kind, InputDeviceId id,
    std::uint32_t button, bool pressed)
{
    return InputEvent{InputEventType::ControlChanged, kind, id,
        InputControlKind::Button, button, pressed, 0.0F};
}

InputEvent axis_event(InputDeviceKind kind, InputDeviceId id,
    std::uint32_t axis, float value)
{
    return InputEvent{InputEventType::ControlChanged, kind, id,
        InputControlKind::Axis, axis, false, value};
}

void test_simultaneous_keyboard_players()
{
    InputManager manager{};
    const GameSession session{};
    manager.handle_event(key_event(input_code::left_arrow, true));
    manager.handle_event(key_event(default_key::p1_light_punch, true));
    manager.handle_event(key_event(static_cast<std::uint32_t>('D'), true));
    manager.handle_event(key_event(default_key::p2_heavy_kick, true));

    const FrameInput frame = manager.sample_frame(1U, session);
    CHECK(frame.player_one.left);
    CHECK(frame.player_one.light_punch);
    CHECK(frame.player_two.right);
    CHECK(frame.player_two.heavy_kick);
}

void test_six_arcade_buttons_and_custom_bindings()
{
    InputManager manager{};
    const GameSession session{};
    for (const std::uint32_t key : {
        default_key::p1_light_punch, default_key::p1_medium_punch,
        default_key::p1_heavy_punch, default_key::p1_light_kick,
        default_key::p1_medium_kick, default_key::p1_heavy_kick}) {
        manager.handle_event(key_event(key, true));
    }
    FrameInput frame = manager.sample_frame(1U, session);
    CHECK(frame.player_one.light_punch);
    CHECK(frame.player_one.medium_punch);
    CHECK(frame.player_one.heavy_punch);
    CHECK(frame.player_one.light_kick);
    CHECK(frame.player_one.medium_kick);
    CHECK(frame.player_one.heavy_kick);

    manager.clear_bindings(PlayerId::One);
    manager.bind(PlayerId::One, InputBinding{InputAction::LightPunch,
        InputDeviceKind::Keyboard, InputControlKind::Key, static_cast<std::uint32_t>('Z'), 0});
    manager.handle_event(key_event(default_key::p1_light_punch, false));
    manager.handle_event(key_event(static_cast<std::uint32_t>('Z'), true));
    frame = manager.sample_frame(2U, session);
    CHECK(frame.player_one.light_punch);
    CHECK(!frame.player_one.medium_punch);
}

void test_input_buffer_and_release()
{
    InputManager manager{};
    manager.set_profile_config(PlayerId::One, InputProfileConfig{0.55F, 0.20F, 3U});
    const GameSession session{};
    manager.handle_event(key_event(default_key::p1_light_kick, true));
    CHECK(manager.sample_frame(10U, session).player_one.light_kick);
    CHECK(manager.sample_frame(12U, session).player_one.light_kick);
    manager.consume(PlayerId::One, InputAction::LightKick);
    CHECK(!manager.sample_frame(12U, session).player_one.light_kick);

    manager.handle_event(key_event(default_key::p1_light_kick, false));
    manager.sample_frame(13U, session);
    manager.handle_event(key_event(default_key::p1_medium_kick, true));
    CHECK(manager.sample_frame(14U, session).player_one.medium_kick);
    CHECK(!manager.sample_frame(18U, session).player_one.medium_kick);
}

void test_direction_reversal_and_relative_facing()
{
    InputManager manager{};
    const GameSession session{};
    manager.handle_event(key_event(input_code::left_arrow, true));
    CHECK(manager.sample_frame(1U, session).player_one.left);
    manager.handle_event(key_event(input_code::left_arrow, false));
    manager.handle_event(key_event(input_code::right_arrow, true));
    const FrameInput frame = manager.sample_frame(2U, session);
    CHECK(!frame.player_one.left);
    CHECK(frame.player_one.right);
    CHECK(frame.player_one.up == false);
}

void test_controller_hotplug_assignment_and_axes()
{
    InputManager manager{};
    const GameSession session{};
    manager.handle_event(device_event(InputEventType::DeviceConnected,
        InputDeviceKind::Gamepad, 41U));
    manager.handle_event(device_event(InputEventType::DeviceConnected,
        InputDeviceKind::Joystick, 77U));
    CHECK(manager.connected_controller_count() == 2U);
    CHECK(manager.assigned_controller(PlayerId::One) == 41U);
    CHECK(manager.assigned_controller(PlayerId::Two) == 77U);
    CHECK(manager.is_connected(41U));

    manager.handle_event(device_event(InputEventType::DeviceConnected,
        InputDeviceKind::Gamepad, 90U));
    CHECK(manager.connected_controller_count() == 2U);
    manager.handle_event(axis_event(InputDeviceKind::Gamepad, 41U, input_code::axis_x, 0.40F));
    FrameInput frame = manager.sample_frame(1U, session);
    CHECK(!frame.player_one.right);
    manager.handle_event(axis_event(InputDeviceKind::Gamepad, 41U, input_code::axis_x, 0.80F));
    frame = manager.sample_frame(2U, session);
    CHECK(frame.player_one.right);

    manager.handle_event(device_event(InputEventType::DeviceDisconnected,
        InputDeviceKind::Gamepad, 41U));
    CHECK(!manager.is_connected(41U));
    CHECK(!manager.assigned_controller(PlayerId::One).has_value());
    CHECK(manager.assigned_controller(PlayerId::Two) == 77U);
    CHECK(manager.connected_controller_count() == 1U);
    CHECK(!manager.sample_frame(3U, session).player_one.right);
}

void test_controller_buttons_and_manual_assignment()
{
    InputManager manager{};
    const GameSession session{};
    manager.handle_event(device_event(InputEventType::DeviceConnected,
        InputDeviceKind::Joystick, 3U));
    manager.handle_event(device_event(InputEventType::DeviceConnected,
        InputDeviceKind::Gamepad, 9U));
    CHECK(manager.assign_controller(PlayerId::One, 9U));
    CHECK(manager.assign_controller(PlayerId::Two, 3U));
    CHECK(manager.assigned_controller(PlayerId::One) == 9U);
    CHECK(manager.assigned_controller(PlayerId::Two) == 3U);

    manager.handle_event(button_event(InputDeviceKind::Gamepad, 9U,
        input_code::gamepad_south, true));
    manager.handle_event(button_event(InputDeviceKind::Joystick, 3U,
        input_code::gamepad_right_shoulder, true));
    const FrameInput frame = manager.sample_frame(1U, session);
    CHECK(frame.player_one.light_punch);
    CHECK(frame.player_two.heavy_kick);
}

void test_controller_reassignment_clears_old_buffered_buttons()
{
    InputManager manager{};
    const GameSession session{};
    manager.handle_event(device_event(InputEventType::DeviceConnected,
        InputDeviceKind::Gamepad, 101U));
    manager.handle_event(device_event(InputEventType::DeviceConnected,
        InputDeviceKind::Gamepad, 202U));
    manager.handle_event(button_event(InputDeviceKind::Gamepad, 101U,
        input_code::gamepad_south, true));
    CHECK(manager.sample_frame(1U, session).player_one.light_punch);

    CHECK(manager.assign_controller(PlayerId::One, 202U));
    CHECK(!manager.sample_frame(2U, session).player_one.light_punch);
    manager.unassign_controller(PlayerId::One);
    CHECK(!manager.sample_frame(3U, session).player_one.light_punch);
}

void test_quarter_circle_forward_with_button()
{
    InputManager manager{};
    const GameSession session{};
    const SpecialMovePattern quarter_circle{
        "quarter-circle-forward",
        {
            MoveStep{RelativeDirection::Down, {}},
            MoveStep{RelativeDirection::DownForward, {}},
            MoveStep{RelativeDirection::Forward, {InputAction::LightPunch}}
        },
        6U,
        12U
    };
    manager.handle_event(key_event(input_code::down_arrow, true));
    manager.sample_frame(1U, session);
    manager.handle_event(key_event(input_code::right_arrow, true));
    manager.sample_frame(2U, session);
    manager.handle_event(key_event(input_code::down_arrow, false));
    manager.handle_event(key_event(default_key::p1_light_punch, true));
    manager.sample_frame(3U, session);
    CHECK(manager.recognizes(PlayerId::One, quarter_circle, 3U));
    CHECK(!manager.recognizes(PlayerId::One, quarter_circle, 30U));
}

void test_half_circle_back_and_reverse_direction()
{
    InputManager manager{};
    const GameSession session{};
    const SpecialMovePattern half_circle_back{
        "half-circle-back",
        {
            MoveStep{RelativeDirection::Back, {}},
            MoveStep{RelativeDirection::DownBack, {}},
            MoveStep{RelativeDirection::Down, {}},
            MoveStep{RelativeDirection::DownForward, {}},
            MoveStep{RelativeDirection::Forward, {InputAction::HeavyKick}}
        },
        5U,
        20U
    };
    manager.handle_event(key_event(input_code::left_arrow, true));
    manager.sample_frame(1U, session);
    manager.handle_event(key_event(input_code::down_arrow, true));
    manager.sample_frame(2U, session);
    manager.handle_event(key_event(input_code::left_arrow, false));
    manager.sample_frame(3U, session);
    manager.handle_event(key_event(input_code::right_arrow, true));
    manager.sample_frame(4U, session);
    manager.handle_event(key_event(input_code::down_arrow, false));
    manager.handle_event(key_event(default_key::p1_heavy_kick, true));
    manager.sample_frame(5U, session);
    CHECK(manager.recognizes(PlayerId::One, half_circle_back, 5U));
}

void test_multi_button_special_chord()
{
    InputManager manager{};
    const GameSession session{};
    const SpecialMovePattern chord{
        "down-forward-double-button",
        {
            MoveStep{RelativeDirection::Down, {}},
            MoveStep{RelativeDirection::Forward,
                {InputAction::LightPunch, InputAction::LightKick}}
        },
        4U,
        8U
    };
    manager.handle_event(key_event(input_code::down_arrow, true));
    manager.sample_frame(1U, session);
    manager.handle_event(key_event(input_code::down_arrow, false));
    manager.handle_event(key_event(input_code::right_arrow, true));
    manager.handle_event(key_event(default_key::p1_light_punch, true));
    manager.handle_event(key_event(default_key::p1_light_kick, true));
    manager.sample_frame(2U, session);
    CHECK(manager.recognizes(PlayerId::One, chord, 2U));
}

void test_session_dispatch_integration()
{
    InputManager manager{};
    GameSession session{};
    manager.handle_event(key_event(default_key::p1_confirm, true));
    manager.dispatch_frame(1U, session);
    CHECK(session.state() == GameState::CharacterSelect);

    manager.handle_event(key_event(default_key::p1_confirm, false));
    manager.handle_event(key_event(default_key::p2_confirm, false));
    manager.dispatch_frame(2U, session);
    manager.handle_event(key_event(default_key::p1_confirm, true));
    manager.handle_event(key_event(default_key::p2_confirm, true));
    manager.dispatch_frame(3U, session);
    CHECK(session.state() == GameState::FightIntro);
}

struct TestCase {
    std::string_view name;
    void (*run)();
};

} // namespace

int main()
{
    constexpr TestCase tests[]{
        {"simultaneous P1/P2 keyboard input", test_simultaneous_keyboard_players},
        {"six arcade buttons and remapping", test_six_arcade_buttons_and_custom_bindings},
        {"input buffer and release", test_input_buffer_and_release},
        {"direction reversal", test_direction_reversal_and_relative_facing},
        {"controller hotplug and axis sensitivity", test_controller_hotplug_assignment_and_axes},
        {"controller buttons and assignment", test_controller_buttons_and_manual_assignment},
        {"controller reassignment clears stale buffer", test_controller_reassignment_clears_old_buffered_buttons},
        {"quarter-circle recognition", test_quarter_circle_forward_with_button},
        {"half-circle back recognition", test_half_circle_back_and_reverse_direction},
        {"multi-button special chord", test_multi_button_special_chord},
        {"GameSession frame dispatch", test_session_dispatch_integration}
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

    std::cout << passed << " input tests passed\n";
    return 0;
}
