#include "software_renderer.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace independent_fighter {
namespace {

std::size_t pixel_index(int width, int x, int y)
{
    return static_cast<std::size_t>(y) * static_cast<std::size_t>(width)
        + static_cast<std::size_t>(x);
}

std::size_t checked_area(int width, int height)
{
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("frame dimensions must be positive");
    }
    const auto unsigned_width = static_cast<std::size_t>(width);
    const auto unsigned_height = static_cast<std::size_t>(height);
    if (unsigned_width > std::numeric_limits<std::size_t>::max() / unsigned_height) {
        throw std::length_error("frame dimensions overflow addressable memory");
    }
    return unsigned_width * unsigned_height;
}

Color blend(Color source, Color destination) noexcept
{
    if (source.a == 255U) {
        return source;
    }
    if (source.a == 0U) {
        return destination;
    }

    const std::uint32_t alpha = source.a;
    const std::uint32_t inverse_alpha = 255U - alpha;
    const auto blend_channel = [alpha, inverse_alpha](std::uint8_t foreground,
                                   std::uint8_t background) {
        return static_cast<std::uint8_t>(
            (static_cast<std::uint32_t>(foreground) * alpha
                + static_cast<std::uint32_t>(background) * inverse_alpha + 127U)
            / 255U);
    };
    return Color{
        blend_channel(source.r, destination.r),
        blend_channel(source.g, destination.g),
        blend_channel(source.b, destination.b),
        255U
    };
}

std::uint8_t layer_value(RenderLayer layer) noexcept
{
    return static_cast<std::uint8_t>(layer);
}

} // namespace

bool Texture::valid() const noexcept
{
    if (width <= 0 || height <= 0) {
        return false;
    }
    const auto unsigned_width = static_cast<std::size_t>(width);
    const auto unsigned_height = static_cast<std::size_t>(height);
    return unsigned_width <= std::numeric_limits<std::size_t>::max() / unsigned_height
        && pixels.size() == unsigned_width * unsigned_height;
}

const Color& Texture::pixel(int x, int y) const
{
    if (x < 0 || y < 0 || x >= width || y >= height || !valid()) {
        throw std::out_of_range("texture pixel is outside the image");
    }
    return pixels[pixel_index(width, x, y)];
}

void FrameBuffer::resize(int new_width, int new_height)
{
    pixels.assign(checked_area(new_width, new_height), Color{});
    width = new_width;
    height = new_height;
}

void FrameBuffer::clear(Color color) noexcept
{
    std::fill(pixels.begin(), pixels.end(), color);
}

const Color& FrameBuffer::pixel(int x, int y) const
{
    if (x < 0 || y < 0 || x >= width || y >= height) {
        throw std::out_of_range("frame pixel is outside the buffer");
    }
    return pixels[pixel_index(width, x, y)];
}

Color& FrameBuffer::pixel(int x, int y)
{
    if (x < 0 || y < 0 || x >= width || y >= height) {
        throw std::out_of_range("frame pixel is outside the buffer");
    }
    return pixels[pixel_index(width, x, y)];
}

SoftwareRenderer::SoftwareRenderer(int logical_width, int logical_height)
{
    frame_.resize(logical_width, logical_height);
}

void SoftwareRenderer::render(
    const ResourceManager& resources,
    Color background,
    std::span<const SpriteDrawCommand> commands)
{
    frame_.clear(background);
    std::vector<SpriteDrawCommand> ordered(commands.begin(), commands.end());
    std::stable_sort(ordered.begin(), ordered.end(), [](const auto& left, const auto& right) {
        if (left.layer != right.layer) {
            return layer_value(left.layer) < layer_value(right.layer);
        }
        return left.order < right.order;
    });

    for (const SpriteDrawCommand& command : ordered) {
        const Texture* texture = resources.texture(command.texture);
        if (texture == nullptr) {
            throw std::invalid_argument("sprite references an unloaded texture");
        }
        draw_command(*texture, command);
    }
}

const FrameBuffer& SoftwareRenderer::frame() const noexcept
{
    return frame_;
}

void SoftwareRenderer::draw_command(
    const Texture& texture,
    const SpriteDrawCommand& command)
{
    const RectI source = command.source;
    const RectI destination = command.destination;
    if (!texture.valid() || source.x < 0 || source.y < 0
        || source.width <= 0 || source.height <= 0
        || source.x > texture.width - source.width
        || source.y > texture.height - source.height
        || destination.width <= 0 || destination.height <= 0) {
        throw std::invalid_argument("sprite source or destination rectangle is invalid");
    }

    for (int destination_y = 0; destination_y < destination.height; ++destination_y) {
        const int target_y = destination.y + destination_y;
        if (target_y < 0 || target_y >= frame_.height) {
            continue;
        }
        const int source_y = source.y
            + destination_y * source.height / destination.height;
        for (int destination_x = 0; destination_x < destination.width; ++destination_x) {
            const int target_x = destination.x + destination_x;
            if (target_x < 0 || target_x >= frame_.width) {
                continue;
            }
            const int scaled_source_x = destination_x * source.width / destination.width;
            const int source_x = source.x + (command.flip_horizontal
                ? source.width - 1 - scaled_source_x
                : scaled_source_x);
            Color& target = frame_.pixel(target_x, target_y);
            target = blend(texture.pixel(source_x, source_y), target);
        }
    }
}

} // namespace independent_fighter
