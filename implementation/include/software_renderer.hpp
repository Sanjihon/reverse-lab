#pragma once

#include "graphics_types.hpp"
#include "resource_manager.hpp"

#include <span>

namespace independent_fighter {

class SoftwareRenderer {
public:
    SoftwareRenderer(int logical_width, int logical_height);

    void render(
        const ResourceManager& resources,
        Color background,
        std::span<const SpriteDrawCommand> commands);

    const FrameBuffer& frame() const noexcept;

private:
    void draw_command(const Texture& texture, const SpriteDrawCommand& command);
    FrameBuffer frame_{};
};

} // namespace independent_fighter
