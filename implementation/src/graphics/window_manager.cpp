#include "window_manager.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

namespace independent_fighter {

bool WindowManager::open(WindowConfig config)
{
    if (config.logical_width <= 0 || config.logical_height <= 0
        || config.output_width <= 0 || config.output_height <= 0) {
        return false;
    }

    config_ = std::move(config);
    output_.resize(config_.output_width, config_.output_height);
    open_ = true;
    update_viewport();
    return true;
}

void WindowManager::close() noexcept
{
    open_ = false;
    viewport_ = {};
    output_ = {};
}

bool WindowManager::resize_output(int width, int height)
{
    if (!open_ || width <= 0 || height <= 0) {
        return false;
    }
    config_.output_width = width;
    config_.output_height = height;
    output_.resize(width, height);
    update_viewport();
    return true;
}

bool WindowManager::present(const FrameBuffer& logical_frame)
{
    if (!open_ || logical_frame.width != config_.logical_width
        || logical_frame.height != config_.logical_height
        || logical_frame.pixels.size() != static_cast<std::size_t>(logical_frame.width)
            * static_cast<std::size_t>(logical_frame.height)) {
        return false;
    }

    output_.clear(Color{0U, 0U, 0U, 255U});
    for (int y = 0; y < viewport_.height; ++y) {
        const int source_y = y * logical_frame.height / viewport_.height;
        for (int x = 0; x < viewport_.width; ++x) {
            const int source_x = x * logical_frame.width / viewport_.width;
            output_.pixel(viewport_.x + x, viewport_.y + y)
                = logical_frame.pixel(source_x, source_y);
        }
    }
    return true;
}

bool WindowManager::is_open() const noexcept
{
    return open_;
}

const WindowConfig& WindowManager::config() const noexcept
{
    return config_;
}

const Viewport& WindowManager::viewport() const noexcept
{
    return viewport_;
}

const FrameBuffer& WindowManager::output_frame() const noexcept
{
    return output_;
}

void WindowManager::update_viewport() noexcept
{
    const double scale_x = static_cast<double>(config_.output_width)
        / static_cast<double>(config_.logical_width);
    const double scale_y = static_cast<double>(config_.output_height)
        / static_cast<double>(config_.logical_height);
    double scale = std::min(scale_x, scale_y);
    if (config_.integer_scaling && scale >= 1.0) {
        scale = std::floor(scale);
    }

    viewport_.width = std::max(1, static_cast<int>(std::floor(
        static_cast<double>(config_.logical_width) * scale)));
    viewport_.height = std::max(1, static_cast<int>(std::floor(
        static_cast<double>(config_.logical_height) * scale)));
    viewport_.x = (config_.output_width - viewport_.width) / 2;
    viewport_.y = (config_.output_height - viewport_.height) / 2;
    viewport_.scale = scale;
}

} // namespace independent_fighter
