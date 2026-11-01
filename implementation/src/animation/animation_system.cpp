#include "animation_system.hpp"

#include <algorithm>

namespace independent_fighter {

void AnimationController::play_clip(const AnimationClip& clip)
{
    movement_state_.reset();
    active_clip_ = clip;
    frame_index_ = 0U;
    ticks_in_frame_ = 0U;
    has_active_clip_ = !active_clip_.frames.empty();
    finished_ = false;
}

bool AnimationController::sync_movement(
    MovementState state,
    const std::unordered_map<MovementState, AnimationClip>& movement_clips)
{
    if (movement_state_.has_value() && movement_state_.value() == state) {
        return false;
    }

    movement_state_ = state;
    frame_index_ = 0U;
    ticks_in_frame_ = 0U;
    finished_ = false;
    const auto found = movement_clips.find(state);
    if (found == movement_clips.end() || found->second.frames.empty()) {
        active_clip_ = {};
        has_active_clip_ = false;
        return true;
    }

    active_clip_ = found->second;
    has_active_clip_ = true;
    return true;
}

void AnimationController::advance_ticks(std::uint32_t ticks) noexcept
{
    if (!has_active_clip_ || finished_ || active_clip_.frames.empty()) {
        return;
    }

    while (ticks > 0U && !finished_) {
        const std::uint32_t frame_duration = std::max(
            1U,
            active_clip_.frames[frame_index_].duration_ticks);
        ++ticks_in_frame_;
        --ticks;
        if (ticks_in_frame_ < frame_duration) {
            continue;
        }

        ticks_in_frame_ = 0U;
        if (frame_index_ + 1U < active_clip_.frames.size()) {
            ++frame_index_;
        } else if (active_clip_.looping) {
            frame_index_ = 0U;
        } else {
            finished_ = true;
        }
    }
}

const AnimationFrame* AnimationController::current_frame() const noexcept
{
    if (!has_active_clip_ || active_clip_.frames.empty()) {
        return nullptr;
    }
    return &active_clip_.frames[frame_index_];
}

std::string_view AnimationController::current_name() const noexcept
{
    return has_active_clip_ ? std::string_view(active_clip_.name) : std::string_view{};
}

std::uint32_t AnimationController::frame_index() const noexcept
{
    return frame_index_;
}

bool AnimationController::finished() const noexcept
{
    return finished_;
}

} // namespace independent_fighter
