#pragma once

#include <cstdint>

namespace independent_fighter {

struct FrameAdvance {
    std::uint32_t simulation_steps{};
    double interpolation_alpha{};
    double dropped_seconds{};
};

class GameLoop {
public:
    static constexpr double fixed_step_seconds = 1.0 / 60.0;
    static constexpr std::uint32_t max_steps_per_advance = 8;

    FrameAdvance advance(double elapsed_seconds) noexcept;
    void reset() noexcept;

private:
    double accumulator_seconds_{};
};

} // namespace independent_fighter
