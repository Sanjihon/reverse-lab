#include "input_manager.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace independent_fighter {
namespace {

struct ControlKey {
    InputDeviceKind device_kind{};
    InputDeviceId device_id{};
    InputControlKind control_kind{};
    std::uint32_t code{};

    bool operator==(const ControlKey&) const = default;
};

struct ControlKeyHash {
    std::size_t operator()(const ControlKey& key) const noexcept
    {
        std::size_t hash = static_cast<std::size_t>(key.device_kind);
        hash = hash * 31U + static_cast<std::size_t>(key.device_id);
        hash = hash * 31U + static_cast<std::size_t>(key.control_kind);
        hash = hash * 31U + static_cast<std::size_t>(key.code);
        return hash;
    }
};

struct DirectionSample {
    std::uint64_t tick{};
    RelativeDirection direction{RelativeDirection::Neutral};
    std::unordered_set<InputAction> pressed{};
};

struct PlayerProfile {
    std::vector<InputBinding> bindings{};
    InputProfileConfig config{};
    std::optional<InputDeviceId> controller{};
    std::unordered_set<InputAction> previous_held{};
    std::unordered_map<InputAction, std::uint64_t> buffered{};
    std::deque<DirectionSample> history{};
    RelativeDirection last_direction{RelativeDirection::Neutral};
};

constexpr std::array<InputAction, 15> all_actions{
    InputAction::Left, InputAction::Right, InputAction::Up, InputAction::Down,
    InputAction::LightPunch, InputAction::MediumPunch, InputAction::HeavyPunch,
    InputAction::LightKick, InputAction::MediumKick, InputAction::HeavyKick,
    InputAction::Run, InputAction::Confirm, InputAction::CycleCharacter,
    InputAction::Pause, InputAction::Restart
};

constexpr bool is_direction(InputAction action) noexcept
{
    return action == InputAction::Left || action == InputAction::Right
        || action == InputAction::Up || action == InputAction::Down;
}

constexpr bool is_bufferable(InputAction action) noexcept
{
    return !is_direction(action) && action != InputAction::Run;
}

void add_binding(
    PlayerProfile& profile,
    InputAction action,
    InputDeviceKind device,
    InputControlKind control,
    std::uint32_t code,
    int axis_direction = 0)
{
    profile.bindings.push_back(InputBinding{action, device, control, code, axis_direction});
}

void bind_keyboard(PlayerProfile& profile, InputAction action, std::uint32_t key)
{
    add_binding(profile, action, InputDeviceKind::Keyboard, InputControlKind::Key, key);
}

void bind_controller(PlayerProfile& profile, InputAction action, std::uint32_t button)
{
    add_binding(profile, action, InputDeviceKind::Gamepad, InputControlKind::Button, button);
    add_binding(profile, action, InputDeviceKind::Joystick, InputControlKind::Button, button);
}

void bind_axes(PlayerProfile& profile)
{
    for (const InputDeviceKind device : {InputDeviceKind::Gamepad, InputDeviceKind::Joystick}) {
        add_binding(profile, InputAction::Left, device, InputControlKind::Axis,
            input_code::axis_x, -1);
        add_binding(profile, InputAction::Right, device, InputControlKind::Axis,
            input_code::axis_x, 1);
        add_binding(profile, InputAction::Up, device, InputControlKind::Axis,
            input_code::axis_y, -1);
        add_binding(profile, InputAction::Down, device, InputControlKind::Axis,
            input_code::axis_y, 1);
    }
}

RelativeDirection direction_from_actions(
    const std::unordered_set<InputAction>& held,
    Facing facing) noexcept
{
    const int horizontal = (held.contains(InputAction::Right) ? 1 : 0)
        - (held.contains(InputAction::Left) ? 1 : 0);
    const int toward = horizontal * static_cast<int>(facing);
    const int vertical = (held.contains(InputAction::Down) ? 1 : 0)
        - (held.contains(InputAction::Up) ? 1 : 0);

    if (toward == 0 && vertical == 0) return RelativeDirection::Neutral;
    if (toward == 0 && vertical < 0) return RelativeDirection::Up;
    if (toward == 0 && vertical > 0) return RelativeDirection::Down;
    if (toward > 0 && vertical == 0) return RelativeDirection::Forward;
    if (toward < 0 && vertical == 0) return RelativeDirection::Back;
    if (toward > 0 && vertical < 0) return RelativeDirection::UpForward;
    if (toward < 0 && vertical < 0) return RelativeDirection::UpBack;
    if (toward > 0) return RelativeDirection::DownForward;
    return RelativeDirection::DownBack;
}

} // namespace

struct InputManager::Impl {
    std::array<PlayerProfile, 2> profiles{};
    std::unordered_map<InputDeviceId, InputDeviceKind> devices{};
    std::unordered_map<ControlKey, float, ControlKeyHash> controls{};
    std::uint64_t current_tick{};

    static std::size_t index(PlayerId player) noexcept
    {
        return player == PlayerId::One ? 0U : 1U;
    }

    bool binding_active(PlayerId player, const InputBinding& binding) const noexcept
    {
        const PlayerProfile& profile = profiles[index(player)];
        InputDeviceId device_id = 0U;
        if (binding.device_kind != InputDeviceKind::Keyboard) {
            if (!profile.controller.has_value()) {
                return false;
            }
            device_id = *profile.controller;
            const auto connected = devices.find(device_id);
            if (connected == devices.end() || connected->second != binding.device_kind) {
                return false;
            }
        }

        const ControlKey key{binding.device_kind, device_id, binding.control_kind, binding.code};
        const auto value = controls.find(key);
        if (value == controls.end()) {
            return false;
        }
        if (binding.control_kind == InputControlKind::Axis) {
            const float threshold = std::max(profile.config.deadzone, profile.config.sensitivity);
            return static_cast<float>(binding.axis_direction) * value->second >= threshold;
        }
        return value->second > 0.5F;
    }

    std::unordered_set<InputAction> held_actions(PlayerId player) const
    {
        std::unordered_set<InputAction> held;
        for (const InputBinding& binding : profiles[index(player)].bindings) {
            if (binding_active(player, binding)) {
                held.insert(binding.action);
            }
        }
        return held;
    }

    void record_sample(
        PlayerId player,
        std::uint64_t tick,
        const std::unordered_set<InputAction>& held,
        Facing facing)
    {
        PlayerProfile& profile = profiles[index(player)];
        std::unordered_set<InputAction> pressed;
        for (const InputAction action : held) {
            if (!profile.previous_held.contains(action)) {
                pressed.insert(action);
                if (is_bufferable(action)) {
                    profile.buffered[action] = tick + profile.config.buffer_ticks;
                }
            }
        }
        profile.previous_held = held;

        const RelativeDirection direction = direction_from_actions(held, facing);
        if (direction != profile.last_direction || !pressed.empty()) {
            profile.history.push_back(DirectionSample{tick, direction, std::move(pressed)});
            profile.last_direction = direction;
        }
        while (!profile.history.empty()
            && tick > profile.history.front().tick
            && tick - profile.history.front().tick > 240U) {
            profile.history.pop_front();
        }
        for (auto event = profile.buffered.begin(); event != profile.buffered.end();) {
            if (tick > event->second) {
                event = profile.buffered.erase(event);
            } else {
                ++event;
            }
        }
    }

    bool buffered(PlayerId player, InputAction action, std::uint64_t tick) const noexcept
    {
        const auto found = profiles[index(player)].buffered.find(action);
        return found != profiles[index(player)].buffered.end() && tick <= found->second;
    }
};

InputManager::InputManager()
    : impl_(std::make_unique<Impl>())
{
    reset_default_profiles();
}

InputManager::~InputManager() = default;
InputManager::InputManager(InputManager&&) noexcept = default;
InputManager& InputManager::operator=(InputManager&&) noexcept = default;

void InputManager::reset_default_profiles()
{
    for (PlayerProfile& profile : impl_->profiles) {
        profile = {};
    }

    PlayerProfile& player_one = impl_->profiles[0];
    bind_keyboard(player_one, InputAction::Left, input_code::left_arrow);
    bind_keyboard(player_one, InputAction::Right, input_code::right_arrow);
    bind_keyboard(player_one, InputAction::Up, input_code::up_arrow);
    bind_keyboard(player_one, InputAction::Down, input_code::down_arrow);
    bind_keyboard(player_one, InputAction::LightPunch, default_key::p1_light_punch);
    bind_keyboard(player_one, InputAction::MediumPunch, default_key::p1_medium_punch);
    bind_keyboard(player_one, InputAction::HeavyPunch, default_key::p1_heavy_punch);
    bind_keyboard(player_one, InputAction::LightKick, default_key::p1_light_kick);
    bind_keyboard(player_one, InputAction::MediumKick, default_key::p1_medium_kick);
    bind_keyboard(player_one, InputAction::HeavyKick, default_key::p1_heavy_kick);
    bind_keyboard(player_one, InputAction::Confirm, default_key::p1_confirm);
    bind_keyboard(player_one, InputAction::Run, default_key::run_modifier);
    bind_keyboard(player_one, InputAction::CycleCharacter, default_key::p1_cycle);
    bind_controller(player_one, InputAction::LightPunch, input_code::gamepad_south);
    bind_controller(player_one, InputAction::MediumPunch, input_code::gamepad_east);
    bind_controller(player_one, InputAction::HeavyPunch, input_code::gamepad_west);
    bind_controller(player_one, InputAction::LightKick, input_code::gamepad_north);
    bind_controller(player_one, InputAction::MediumKick, input_code::gamepad_left_shoulder);
    bind_controller(player_one, InputAction::HeavyKick, input_code::gamepad_right_shoulder);
    bind_controller(player_one, InputAction::Confirm, input_code::gamepad_confirm);
    bind_controller(player_one, InputAction::CycleCharacter, input_code::gamepad_cycle);
    bind_controller(player_one, InputAction::Pause, input_code::gamepad_pause);
    bind_controller(player_one, InputAction::Restart, input_code::gamepad_restart);
    bind_controller(player_one, InputAction::Up, input_code::gamepad_dpad_up);
    bind_controller(player_one, InputAction::Down, input_code::gamepad_dpad_down);
    bind_controller(player_one, InputAction::Left, input_code::gamepad_dpad_left);
    bind_controller(player_one, InputAction::Right, input_code::gamepad_dpad_right);
    bind_axes(player_one);

    PlayerProfile& player_two = impl_->profiles[1];
    bind_keyboard(player_two, InputAction::Left, static_cast<std::uint32_t>('A'));
    bind_keyboard(player_two, InputAction::Right, static_cast<std::uint32_t>('D'));
    bind_keyboard(player_two, InputAction::Up, static_cast<std::uint32_t>('W'));
    bind_keyboard(player_two, InputAction::Down, static_cast<std::uint32_t>('S'));
    bind_keyboard(player_two, InputAction::LightPunch, default_key::p2_light_punch);
    bind_keyboard(player_two, InputAction::MediumPunch, default_key::p2_medium_punch);
    bind_keyboard(player_two, InputAction::HeavyPunch, default_key::p2_heavy_punch);
    bind_keyboard(player_two, InputAction::LightKick, default_key::p2_light_kick);
    bind_keyboard(player_two, InputAction::MediumKick, default_key::p2_medium_kick);
    bind_keyboard(player_two, InputAction::HeavyKick, default_key::p2_heavy_kick);
    bind_keyboard(player_two, InputAction::Confirm, default_key::p2_confirm);
    bind_keyboard(player_two, InputAction::Run, default_key::run_modifier);
    bind_keyboard(player_two, InputAction::CycleCharacter, default_key::p2_cycle);
    bind_controller(player_two, InputAction::LightPunch, input_code::gamepad_south);
    bind_controller(player_two, InputAction::MediumPunch, input_code::gamepad_east);
    bind_controller(player_two, InputAction::HeavyPunch, input_code::gamepad_west);
    bind_controller(player_two, InputAction::LightKick, input_code::gamepad_north);
    bind_controller(player_two, InputAction::MediumKick, input_code::gamepad_left_shoulder);
    bind_controller(player_two, InputAction::HeavyKick, input_code::gamepad_right_shoulder);
    bind_controller(player_two, InputAction::Confirm, input_code::gamepad_confirm);
    bind_controller(player_two, InputAction::CycleCharacter, input_code::gamepad_cycle);
    bind_controller(player_two, InputAction::Pause, input_code::gamepad_pause);
    bind_controller(player_two, InputAction::Restart, input_code::gamepad_restart);
    bind_controller(player_two, InputAction::Up, input_code::gamepad_dpad_up);
    bind_controller(player_two, InputAction::Down, input_code::gamepad_dpad_down);
    bind_controller(player_two, InputAction::Left, input_code::gamepad_dpad_left);
    bind_controller(player_two, InputAction::Right, input_code::gamepad_dpad_right);
    bind_axes(player_two);

    for (PlayerProfile& profile : impl_->profiles) {
        bind_keyboard(profile, InputAction::Pause, default_key::pause);
        bind_keyboard(profile, InputAction::Restart, default_key::restart);
    }
}

void InputManager::clear_runtime_state() noexcept
{
    impl_->devices.clear();
    impl_->controls.clear();
    impl_->current_tick = 0U;
    for (PlayerProfile& profile : impl_->profiles) {
        profile.controller.reset();
        profile.previous_held.clear();
        profile.buffered.clear();
        profile.history.clear();
        profile.last_direction = RelativeDirection::Neutral;
    }
}

void InputManager::bind(PlayerId player, InputBinding binding)
{
    PlayerProfile& profile = impl_->profiles[Impl::index(player)];
    const auto duplicate = std::find_if(profile.bindings.begin(), profile.bindings.end(),
        [&binding](const InputBinding& existing) {
            return existing.action == binding.action
                && existing.device_kind == binding.device_kind
                && existing.control_kind == binding.control_kind
                && existing.code == binding.code
                && existing.axis_direction == binding.axis_direction;
        });
    if (duplicate == profile.bindings.end()) {
        profile.bindings.push_back(binding);
    }
}

void InputManager::clear_bindings(PlayerId player)
{
    PlayerProfile& profile = impl_->profiles[Impl::index(player)];
    profile.bindings.clear();
    profile.previous_held.clear();
    profile.buffered.clear();
}

void InputManager::set_profile_config(PlayerId player, InputProfileConfig config)
{
    if (!std::isfinite(config.sensitivity) || !std::isfinite(config.deadzone)) {
        config.sensitivity = 0.55F;
        config.deadzone = 0.20F;
    }
    config.sensitivity = std::clamp(config.sensitivity, 0.0F, 1.0F);
    config.deadzone = std::clamp(config.deadzone, 0.0F, 1.0F);
    impl_->profiles[Impl::index(player)].config = config;
}

bool InputManager::assign_controller(PlayerId player, InputDeviceId device_id)
{
    const auto device = impl_->devices.find(device_id);
    if (device == impl_->devices.end() || device->second == InputDeviceKind::Keyboard) {
        return false;
    }
    for (PlayerProfile& profile : impl_->profiles) {
        if (profile.controller == device_id) {
            profile.controller.reset();
            profile.previous_held.clear();
            profile.buffered.clear();
            profile.history.clear();
            profile.last_direction = RelativeDirection::Neutral;
        }
    }
    PlayerProfile& destination = impl_->profiles[Impl::index(player)];
    destination.previous_held.clear();
    destination.buffered.clear();
    destination.history.clear();
    destination.last_direction = RelativeDirection::Neutral;
    destination.controller = device_id;
    destination.previous_held = impl_->held_actions(player);
    return true;
}

void InputManager::unassign_controller(PlayerId player) noexcept
{
    PlayerProfile& profile = impl_->profiles[Impl::index(player)];
    profile.controller.reset();
    profile.previous_held.clear();
    profile.buffered.clear();
    profile.history.clear();
    profile.last_direction = RelativeDirection::Neutral;
}

std::optional<InputDeviceId> InputManager::assigned_controller(PlayerId player) const noexcept
{
    return impl_->profiles[Impl::index(player)].controller;
}

bool InputManager::is_connected(InputDeviceId device_id) const noexcept
{
    return impl_->devices.contains(device_id);
}

std::size_t InputManager::connected_controller_count() const noexcept
{
    return impl_->devices.size();
}

void InputManager::handle_event(const InputEvent& event)
{
    if (event.type == InputEventType::DeviceConnected) {
        if (event.device_kind == InputDeviceKind::Keyboard
            || impl_->devices.contains(event.device_id)
            || impl_->devices.size() >= 2U) {
            return;
        }
        impl_->devices.emplace(event.device_id, event.device_kind);
        for (PlayerProfile& profile : impl_->profiles) {
            if (!profile.controller.has_value()) {
                profile.controller = event.device_id;
                break;
            }
        }
        return;
    }

    if (event.type == InputEventType::DeviceDisconnected) {
        impl_->devices.erase(event.device_id);
        for (PlayerProfile& profile : impl_->profiles) {
            if (profile.controller == event.device_id) {
                profile.controller.reset();
                profile.previous_held.clear();
                profile.buffered.clear();
                profile.history.clear();
                profile.last_direction = RelativeDirection::Neutral;
            }
        }
        for (auto control = impl_->controls.begin(); control != impl_->controls.end();) {
            if (control->first.device_id == event.device_id
                && control->first.device_kind != InputDeviceKind::Keyboard) {
                control = impl_->controls.erase(control);
            } else {
                ++control;
            }
        }
        return;
    }

    if (event.device_kind != InputDeviceKind::Keyboard) {
        const auto device = impl_->devices.find(event.device_id);
        if (device == impl_->devices.end() || device->second != event.device_kind) {
            return;
        }
    }

    const InputDeviceId device_id = event.device_kind == InputDeviceKind::Keyboard
        ? 0U
        : event.device_id;
    const ControlKey key{event.device_kind, device_id, event.control_kind, event.code};
    if (event.control_kind == InputControlKind::Axis) {
        const float value = std::isfinite(event.axis_value)
            ? std::clamp(event.axis_value, -1.0F, 1.0F)
            : 0.0F;
        if (std::abs(value) < 0.0001F) {
            impl_->controls.erase(key);
        } else {
            impl_->controls.insert_or_assign(key, value);
        }
    } else if (event.pressed) {
        impl_->controls.insert_or_assign(key, 1.0F);
    } else {
        impl_->controls.erase(key);
    }
}

FrameInput InputManager::sample_frame(std::uint64_t tick, const GameSession& session)
{
    impl_->current_tick = tick;
    const std::array<PlayerId, 2> ids{PlayerId::One, PlayerId::Two};
    const std::array<Facing, 2> facings{
        session.player(PlayerId::One).facing,
        session.player(PlayerId::Two).facing
    };
    std::array<std::unordered_set<InputAction>, 2> held{};
    for (std::size_t index = 0; index < ids.size(); ++index) {
        held[index] = impl_->held_actions(ids[index]);
        impl_->record_sample(ids[index], tick, held[index], facings[index]);
    }

    const auto active = [&held](std::size_t player_index, InputAction action) {
        return held[player_index].contains(action);
    };
    const auto buffered = [this, tick](PlayerId player, InputAction action) {
        return impl_->buffered(player, action, tick);
    };
    const auto player_input = [&](std::size_t index, PlayerId player) {
        PlayerInput result{};
        result.left = active(index, InputAction::Left);
        result.right = active(index, InputAction::Right);
        result.up = active(index, InputAction::Up);
        result.down = active(index, InputAction::Down);
        result.jump = result.up;
        result.crouch = result.down;
        result.run = active(index, InputAction::Run);
        result.light_punch = buffered(player, InputAction::LightPunch);
        result.medium_punch = buffered(player, InputAction::MediumPunch);
        result.heavy_punch = buffered(player, InputAction::HeavyPunch);
        result.light_kick = buffered(player, InputAction::LightKick);
        result.medium_kick = buffered(player, InputAction::MediumKick);
        result.heavy_kick = buffered(player, InputAction::HeavyKick);
        result.confirm = buffered(player, InputAction::Confirm);
        result.cycle_character = buffered(player, InputAction::CycleCharacter);
        return result;
    };

    FrameInput frame{};
    frame.player_one = player_input(0U, PlayerId::One);
    frame.player_two = player_input(1U, PlayerId::Two);
    frame.confirm = frame.player_one.confirm || frame.player_two.confirm;
    frame.pause = buffered(PlayerId::One, InputAction::Pause)
        || buffered(PlayerId::Two, InputAction::Pause);
    frame.restart = buffered(PlayerId::One, InputAction::Restart)
        || buffered(PlayerId::Two, InputAction::Restart);
    return frame;
}

void InputManager::dispatch_frame(std::uint64_t tick, GameSession& session)
{
    const FrameInput frame = sample_frame(tick, session);
    session.process_frame(frame);
    for (const PlayerId player : {PlayerId::One, PlayerId::Two}) {
        consume(player, InputAction::Confirm);
        consume(player, InputAction::CycleCharacter);
        consume(player, InputAction::Pause);
        consume(player, InputAction::Restart);
    }
}

void InputManager::consume(PlayerId player, InputAction action) noexcept
{
    impl_->profiles[Impl::index(player)].buffered.erase(action);
}

bool InputManager::recognizes(
    PlayerId player,
    const SpecialMovePattern& pattern,
    std::uint64_t current_tick) const noexcept
{
    if (pattern.steps.empty() || pattern.total_window_ticks == 0U) {
        return false;
    }
    const auto& history = impl_->profiles[Impl::index(player)].history;
    std::size_t step_index = pattern.steps.size();
    std::optional<std::uint64_t> last_matched_tick{};

    for (auto sample = history.rbegin(); sample != history.rend(); ++sample) {
        if (current_tick < sample->tick
            || current_tick - sample->tick > pattern.total_window_ticks) {
            break;
        }
        const MoveStep& expected = pattern.steps[step_index - 1U];
        if (expected.direction.has_value() && sample->direction != *expected.direction) {
            continue;
        }
        bool buttons_present = true;
        for (const InputAction button : expected.buttons) {
            if (!sample->pressed.contains(button)) {
                buttons_present = false;
                break;
            }
        }
        if (!buttons_present) {
            continue;
        }
        if (last_matched_tick.has_value()
            && *last_matched_tick - sample->tick > pattern.max_gap_ticks) {
            return false;
        }
        last_matched_tick = sample->tick;
        --step_index;
        if (step_index == 0U) {
            return true;
        }
    }
    return false;
}

std::size_t InputManager::buffered_action_count(PlayerId player) const noexcept
{
    return impl_->profiles[Impl::index(player)].buffered.size();
}

} // namespace independent_fighter
