#pragma once

#include "animation_system.hpp"
#include "graphics_types.hpp"

#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>

namespace independent_fighter {

class ResourceManager {
public:
    TextureHandle load_ppm(const std::filesystem::path& path);
    TextureHandle create_placeholder(
        std::string key,
        int width,
        int height,
        Color first,
        Color second);
    const Texture* texture(TextureHandle handle) const noexcept;
    bool unload_texture(TextureHandle handle) noexcept;

    const AnimationClip& load_animation_definition(const std::filesystem::path& path);
    const AnimationClip* animation(std::string_view name) const noexcept;
    bool unload_animation(std::string_view name) noexcept;

    void clear() noexcept;
    std::size_t texture_count() const noexcept;
    std::size_t animation_count() const noexcept;

private:
    TextureHandle store_texture(std::string key, Texture texture);

    std::uint32_t next_texture_id_{1U};
    std::unordered_map<std::uint32_t, Texture> textures_{};
    std::unordered_map<std::string, TextureHandle> texture_keys_{};
    std::unordered_map<std::string, AnimationClip> animations_{};
};

} // namespace independent_fighter
