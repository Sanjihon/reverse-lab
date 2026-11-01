#include "frame_pacer.hpp"

#include <stdexcept>

namespace independent_fighter {
namespace {

FramePacer::Duration frame_interval(std::uint32_t refresh_hz)
{
    if (refresh_hz == 0U) {
        throw std::invalid_argument("refresh rate must be positive");
    }
    const auto interval = std::chrono::duration_cast<FramePacer::Duration>(
        std::chrono::duration<double>(1.0 / static_cast<double>(refresh_hz)));
    if (interval <= FramePacer::Duration::zero()) {
        throw std::invalid_argument("refresh rate exceeds clock resolution");
    }
    return interval;
}

} // namespace

FramePacer::FramePacer(std::uint32_t refresh_hz)
    : refresh_hz_(refresh_hz),
      interval_(frame_interval(refresh_hz))
{
}

FramePacer::TimePoint FramePacer::schedule_next(TimePoint now) noexcept
{
    if (!scheduled_ || next_frame_ <= now) {
        next_frame_ = now + interval_;
    } else {
        next_frame_ += interval_;
    }
    scheduled_ = true;
    return next_frame_;
}

FramePacer::Duration FramePacer::time_until_next(TimePoint now) const noexcept
{
    if (!scheduled_ || next_frame_ <= now) {
        return Duration::zero();
    }
    return next_frame_ - now;
}

void FramePacer::reset() noexcept
{
    scheduled_ = false;
    next_frame_ = TimePoint{};
}

std::uint32_t FramePacer::refresh_hz() const noexcept
{
    return refresh_hz_;
}

} // namespace independent_fighter
