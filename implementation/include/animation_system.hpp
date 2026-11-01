#pragma once

#include "game_types.hpp"
#include "graphics_types.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace independent_fighter {

struct AnimationFrame {
    RectI source{};
    std::uint32_t duration_ticks{1U};
};

struct AnimationClip {
    std::string name;
    std::vector<AnimationFrame> frames;
    bool looping{true};
};

class AnimationController {
public:
    void play_clip(const AnimationClip& clip);
    bool sync_movement(
        MovementState state,
        const std::unordered_map<MovementState, AnimationClip>& movement_clips);
    void advance_ticks(std::uint32_t ticks = 1U) noexcept;

    const AnimationFrame* current_frame() const noexcept;
    std::string_view current_name() const noexcept;
    std::uint32_t frame_index() const noexcept;
    bool finished() const noexcept;

private:
    std::optional<MovementState> movement_state_{};
    AnimationClip active_clip_{};
    std::uint32_t frame_index_{};
    std::uint32_t ticks_in_frame_{};
    bool has_active_clip_{};
    bool finished_{};
};

} // namespace independent_fighter
