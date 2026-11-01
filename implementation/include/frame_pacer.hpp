#pragma once

#include <chrono>
#include <cstdint>

namespace independent_fighter {

class FramePacer {
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Duration = Clock::duration;

    explicit FramePacer(std::uint32_t refresh_hz = 60U);

    TimePoint schedule_next(TimePoint now) noexcept;
    Duration time_until_next(TimePoint now) const noexcept;
    void reset() noexcept;
    std::uint32_t refresh_hz() const noexcept;

private:
    std::uint32_t refresh_hz_{};
    Duration interval_{};
    TimePoint next_frame_{};
    bool scheduled_{};
};

} // namespace independent_fighter
