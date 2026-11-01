#include "game_loop.hpp"

#include <algorithm>
#include <cmath>

namespace independent_fighter {
namespace {

constexpr double max_frame_seconds = 0.25;
constexpr double step_epsilon = 1.0e-12;

} // namespace

FrameAdvance GameLoop::advance(double elapsed_seconds) noexcept
{
    if (!std::isfinite(elapsed_seconds) || elapsed_seconds <= 0.0) {
        return FrameAdvance{0U, accumulator_seconds_ / fixed_step_seconds, 0.0};
    }

    const double accepted_seconds = std::min(elapsed_seconds, max_frame_seconds);
    double dropped_seconds = elapsed_seconds - accepted_seconds;
    accumulator_seconds_ += accepted_seconds;

    std::uint32_t steps = 0U;
    while (accumulator_seconds_ + step_epsilon >= fixed_step_seconds
        && steps < max_steps_per_advance) {
        accumulator_seconds_ -= fixed_step_seconds;
        ++steps;
    }

    if (steps == max_steps_per_advance
        && accumulator_seconds_ >= fixed_step_seconds) {
        const double remainder = std::fmod(accumulator_seconds_, fixed_step_seconds);
        dropped_seconds += accumulator_seconds_ - remainder;
        accumulator_seconds_ = remainder;
    }

    return FrameAdvance{
        steps,
        accumulator_seconds_ / fixed_step_seconds,
        dropped_seconds
    };
}

void GameLoop::reset() noexcept
{
    accumulator_seconds_ = 0.0;
}

} // namespace independent_fighter
