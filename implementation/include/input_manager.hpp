#pragma once

#include "game_session.hpp"
#include "game_types.hpp"

#include <cstdint>
#include <deque>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace independent_fighter {

using InputDeviceId = std::uint32_t;

enum class InputDeviceKind : std::uint8_t {
    Keyboard,
    Gamepad,
    Joystick
};

enum class InputControlKind : std::uint8_t {
    Key,
    Button,
    Axis
};
enum class InputEventType : std::uint8_t {
    DeviceConnected,
    DeviceDisconnected,
    ControlChanged
};
enum class InputAction : std::uint8_t {
    Left,
    Right,
    Up,
    Down,
    LightPunch,
    MediumPunch,
    HeavyPunch,
    LightKick,
    MediumKick,
    HeavyKick,
    Run,
    Confirm,
    CycleCharacter,
    Pause,
    Restart
};

enum class RelativeDirection : std::uint8_t {
    Neutral,
    Forward,
    Back,
    Up,
    Down,
    UpForward,
    UpBack,
    DownForward,
    DownBack
};

namespace input_code {
inline constexpr std::uint32_t left_arrow = 0x1001U;
inline constexpr std::uint32_t right_arrow = 0x1002U;
inline constexpr std::uint32_t up_arrow = 0x1003U;
inline constexpr std::uint32_t down_arrow = 0x1004U;
inline constexpr std::uint32_t gamepad_south = 0U;
inline constexpr std::uint32_t gamepad_east = 1U;
inline constexpr std::uint32_t gamepad_west = 2U;
inline constexpr std::uint32_t gamepad_north = 3U;
inline constexpr std::uint32_t gamepad_left_shoulder = 4U;
inline constexpr std::uint32_t gamepad_right_shoulder = 5U;
inline constexpr std::uint32_t gamepad_confirm = 6U;
inline constexpr std::uint32_t gamepad_cycle = 7U;
inline constexpr std::uint32_t gamepad_pause = 8U;
inline constexpr std::uint32_t gamepad_restart = 9U;
inline constexpr std::uint32_t gamepad_dpad_up = 10U;
inline constexpr std::uint32_t gamepad_dpad_down = 11U;
inline constexpr std::uint32_t gamepad_dpad_left = 12U;
inline constexpr std::uint32_t gamepad_dpad_right = 13U;
inline constexpr std::uint32_t axis_x = 0U;
inline constexpr std::uint32_t axis_y = 1U;
}

struct InputEvent {
    InputEventType type{InputEventType::ControlChanged};
    InputDeviceKind device_kind{InputDeviceKind::Keyboard};
    InputDeviceId device_id{};
    InputControlKind control_kind{InputControlKind::Key};
    std::uint32_t code{};
    bool pressed{};
    float axis_value{};
};

struct InputBinding {
    InputAction action{InputAction::Left};
    InputDeviceKind device_kind{InputDeviceKind::Keyboard};
    InputControlKind control_kind{InputControlKind::Key};
    std::uint32_t code{};
    int axis_direction{};
};

struct InputProfileConfig {
    float sensitivity{0.55F};
    float deadzone{0.20F};
    std::uint32_t buffer_ticks{5U};
};

struct MoveStep {
    std::optional<RelativeDirection> direction{};
    std::vector<InputAction> buttons{};
};

struct SpecialMovePattern {
    std::string name;
    std::vector<MoveStep> steps;
    std::uint32_t max_gap_ticks{12U};
    std::uint32_t total_window_ticks{30U};
};

class InputManager {
public:
    InputManager();
    ~InputManager();
    InputManager(InputManager&&) noexcept;
    InputManager& operator=(InputManager&&) noexcept;
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    void reset_default_profiles();
    void clear_runtime_state() noexcept;
    void bind(PlayerId player, InputBinding binding);
    void clear_bindings(PlayerId player);
    void set_profile_config(PlayerId player, InputProfileConfig config);
    bool assign_controller(PlayerId player, InputDeviceId device_id);
    void unassign_controller(PlayerId player) noexcept;
    std::optional<InputDeviceId> assigned_controller(PlayerId player) const noexcept;
    bool is_connected(InputDeviceId device_id) const noexcept;
    std::size_t connected_controller_count() const noexcept;

    void handle_event(const InputEvent& event);
    FrameInput sample_frame(std::uint64_t tick, const GameSession& session);
    void dispatch_frame(std::uint64_t tick, GameSession& session);
    void consume(PlayerId player, InputAction action) noexcept;
    bool recognizes(PlayerId player, const SpecialMovePattern& pattern,
        std::uint64_t current_tick) const noexcept;
    std::size_t buffered_action_count(PlayerId player) const noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

namespace default_key {
inline constexpr std::uint32_t p1_light_punch = static_cast<std::uint32_t>('J');
inline constexpr std::uint32_t p1_medium_punch = static_cast<std::uint32_t>('K');
inline constexpr std::uint32_t p1_heavy_punch = static_cast<std::uint32_t>('L');
inline constexpr std::uint32_t p1_light_kick = static_cast<std::uint32_t>('U');
inline constexpr std::uint32_t p1_medium_kick = static_cast<std::uint32_t>('I');
inline constexpr std::uint32_t p1_heavy_kick = static_cast<std::uint32_t>('O');
inline constexpr std::uint32_t p2_light_punch = static_cast<std::uint32_t>('F');
inline constexpr std::uint32_t p2_medium_punch = static_cast<std::uint32_t>('G');
inline constexpr std::uint32_t p2_heavy_punch = static_cast<std::uint32_t>('H');
inline constexpr std::uint32_t p2_light_kick = static_cast<std::uint32_t>('R');
inline constexpr std::uint32_t p2_medium_kick = static_cast<std::uint32_t>('T');
inline constexpr std::uint32_t p2_heavy_kick = static_cast<std::uint32_t>('Y');
inline constexpr std::uint32_t p1_confirm = static_cast<std::uint32_t>('1');
inline constexpr std::uint32_t p2_confirm = static_cast<std::uint32_t>('2');
inline constexpr std::uint32_t p1_cycle = static_cast<std::uint32_t>('C');
inline constexpr std::uint32_t p2_cycle = static_cast<std::uint32_t>('V');
inline constexpr std::uint32_t run_modifier = 0x1010U;
inline constexpr std::uint32_t pause = static_cast<std::uint32_t>('P');
inline constexpr std::uint32_t restart = static_cast<std::uint32_t>('N');
}

} // namespace independent_fighter
