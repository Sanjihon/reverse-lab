#pragma once

#include "graphics_types.hpp"

#include <string>

namespace independent_fighter {

struct WindowConfig {
    std::string title{"Independent Fighter"};
    int logical_width{640};
    int logical_height{360};
    int output_width{1280};
    int output_height{720};
    bool integer_scaling{true};
};

struct Viewport {
    int x{};
    int y{};
    int width{};
    int height{};
    double scale{};
};

// Headless window surface. A native SDL window is a separate platform adapter.
class WindowManager {
public:
    bool open(WindowConfig config);
    void close() noexcept;
    bool resize_output(int width, int height);
    bool present(const FrameBuffer& logical_frame);

    bool is_open() const noexcept;
    const WindowConfig& config() const noexcept;
    const Viewport& viewport() const noexcept;
    const FrameBuffer& output_frame() const noexcept;

private:
    void update_viewport() noexcept;

    WindowConfig config_{};
    Viewport viewport_{};
    FrameBuffer output_{};
    bool open_{};
};

} // namespace independent_fighter
