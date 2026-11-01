#include "resource_manager.hpp"

#include <cctype>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace independent_fighter {
namespace {

std::string read_ppm_token(std::istream& input)
{
    std::string token;
    for (;;) {
        const int next = input.peek();
        if (next == std::char_traits<char>::eof()) {
            throw std::runtime_error("unexpected end of PPM header");
        }
        if (std::isspace(static_cast<unsigned char>(next)) != 0) {
            input.get();
            continue;
        }
        if (next == '#') {
            input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        break;
    }

    while (input.good()) {
        const int next = input.peek();
        if (next == std::char_traits<char>::eof()
            || std::isspace(static_cast<unsigned char>(next)) != 0
            || next == '#') {
            break;
        }
        token.push_back(static_cast<char>(input.get()));
    }
    if (token.empty()) {
        throw std::runtime_error("empty PPM header token");
    }
    return token;
}

int parse_positive_int(const std::string& value, const char* field)
{
    std::size_t parsed = 0;
    const int result = std::stoi(value, &parsed);
    if (parsed != value.size() || result <= 0) {
        throw std::runtime_error(std::string("invalid PPM ") + field);
    }
    return result;
}

std::uint32_t parse_positive_uint(const std::string& value, const char* field)
{
    std::size_t parsed = 0;
    const unsigned long result = std::stoul(value, &parsed);
    if (parsed != value.size() || result == 0UL
        || result > static_cast<unsigned long>(std::numeric_limits<std::uint32_t>::max())) {
        throw std::runtime_error(std::string("invalid animation ") + field);
    }
    return static_cast<std::uint32_t>(result);
}

} // namespace

TextureHandle ResourceManager::load_ppm(const std::filesystem::path& path)
{
    const std::string key = std::filesystem::absolute(path).lexically_normal().string();
    const auto cached = texture_keys_.find(key);
    if (cached != texture_keys_.end()) {
        return cached->second;
    }

    std::ifstream input(path, std::ios::binary);
    if (!input) {
        throw std::runtime_error("unable to open PPM texture: " + path.string());
    }
    if (read_ppm_token(input) != "P6") {
        throw std::runtime_error("only binary P6 PPM textures are supported");
    }

    Texture loaded{};
    loaded.width = parse_positive_int(read_ppm_token(input), "width");
    loaded.height = parse_positive_int(read_ppm_token(input), "height");
    if (read_ppm_token(input) != "255") {
        throw std::runtime_error("PPM max channel value must be 255");
    }

    const int separator = input.get();
    if (separator == std::char_traits<char>::eof()
        || std::isspace(static_cast<unsigned char>(separator)) == 0) {
        throw std::runtime_error("missing PPM pixel-data separator");
    }
    if (separator == '\r' && input.peek() == '\n') {
        input.get();
    }

    const auto width = static_cast<std::size_t>(loaded.width);
    const auto height = static_cast<std::size_t>(loaded.height);
    if (width > std::numeric_limits<std::size_t>::max() / height
        || width * height > std::numeric_limits<std::size_t>::max() / 3U) {
        throw std::runtime_error("PPM texture dimensions overflow addressable memory");
    }

    const std::size_t pixel_count = width * height;
    loaded.pixels.resize(pixel_count);
    for (Color& pixel : loaded.pixels) {
        char rgb[3]{};
        input.read(rgb, 3);
        if (input.gcount() != 3) {
            throw std::runtime_error("truncated PPM pixel data");
        }
        pixel = Color{
            static_cast<std::uint8_t>(static_cast<unsigned char>(rgb[0])),
            static_cast<std::uint8_t>(static_cast<unsigned char>(rgb[1])),
            static_cast<std::uint8_t>(static_cast<unsigned char>(rgb[2])),
            255U
        };
    }

    return store_texture(key, std::move(loaded));
}

TextureHandle ResourceManager::create_placeholder(
    std::string key,
    int width,
    int height,
    Color first,
    Color second)
{
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("placeholder dimensions must be positive");
    }
    const auto unsigned_width = static_cast<std::size_t>(width);
    const auto unsigned_height = static_cast<std::size_t>(height);
    if (unsigned_width > std::numeric_limits<std::size_t>::max() / unsigned_height) {
        throw std::length_error("placeholder dimensions overflow addressable memory");
    }

    Texture placeholder{};
    placeholder.width = width;
    placeholder.height = height;
    placeholder.pixels.resize(unsigned_width * unsigned_height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const bool alternate = ((x / 8) + (y / 8)) % 2 != 0;
            const Color color = alternate ? second : first;
            placeholder.pixels[static_cast<std::size_t>(y)
                * static_cast<std::size_t>(width) + static_cast<std::size_t>(x)] = color;
        }
    }
    return store_texture(std::move(key), std::move(placeholder));
}

const Texture* ResourceManager::texture(TextureHandle handle) const noexcept
{
    const auto found = textures_.find(handle.value);
    return found == textures_.end() ? nullptr : &found->second;
}

bool ResourceManager::unload_texture(TextureHandle handle) noexcept
{
    const auto found = textures_.find(handle.value);
    if (found == textures_.end()) {
        return false;
    }

    for (auto key = texture_keys_.begin(); key != texture_keys_.end();) {
        if (key->second == handle) {
            key = texture_keys_.erase(key);
        } else {
            ++key;
        }
    }
    textures_.erase(found);
    return true;
}

const AnimationClip& ResourceManager::load_animation_definition(
    const std::filesystem::path& path)
{
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("unable to open animation definition: " + path.string());
    }

    AnimationClip clip{};
    clip.name = path.stem().string();
    bool loop_seen = false;
    std::string line;
    std::size_t line_number = 0;
    while (std::getline(input, line)) {
        ++line_number;
        const std::size_t comment = line.find('#');
        if (comment != std::string::npos) {
            line.erase(comment);
        }
        std::istringstream fields(line);
        std::string directive;
        if (!(fields >> directive)) {
            continue;
        }

        if (directive == "loop") {
            std::string value;
            std::string extra;
            if (!(fields >> value) || (fields >> extra) || loop_seen) {
                throw std::runtime_error("invalid loop directive at line "
                    + std::to_string(line_number));
            }
            if (value == "true" || value == "1") {
                clip.looping = true;
            } else if (value == "false" || value == "0") {
                clip.looping = false;
            } else {
                throw std::runtime_error("loop must be true or false");
            }
            loop_seen = true;
        } else if (directive == "frame") {
            std::string x_text;
            std::string y_text;
            std::string width_text;
            std::string height_text;
            std::string duration_text;
            std::string extra;
            if (!(fields >> x_text >> y_text >> width_text >> height_text >> duration_text)
                || (fields >> extra)) {
                throw std::runtime_error("invalid frame directive at line "
                    + std::to_string(line_number));
            }
            const int x = std::stoi(x_text);
            const int y = std::stoi(y_text);
            const int width = parse_positive_int(width_text, "frame width");
            const int height = parse_positive_int(height_text, "frame height");
            const std::uint32_t duration = parse_positive_uint(duration_text, "frame duration");
            if (x < 0 || y < 0) {
                throw std::runtime_error("animation frame coordinates must be nonnegative");
            }
            clip.frames.push_back(AnimationFrame{RectI{x, y, width, height}, duration});
        } else {
            throw std::runtime_error("unknown animation directive at line "
                + std::to_string(line_number));
        }
    }

    if (clip.frames.empty()) {
        throw std::runtime_error("animation definition must contain at least one frame");
    }
    const auto inserted = animations_.insert_or_assign(clip.name, std::move(clip));
    return inserted.first->second;
}

const AnimationClip* ResourceManager::animation(std::string_view name) const noexcept
{
    for (const auto& entry : animations_) {
        if (entry.first == name) {
            return &entry.second;
        }
    }
    return nullptr;
}

bool ResourceManager::unload_animation(std::string_view name) noexcept
{
    for (auto entry = animations_.begin(); entry != animations_.end(); ++entry) {
        if (entry->first == name) {
            animations_.erase(entry);
            return true;
        }
    }
    return false;
}

void ResourceManager::clear() noexcept
{
    animations_.clear();
    texture_keys_.clear();
    textures_.clear();
}

std::size_t ResourceManager::texture_count() const noexcept
{
    return textures_.size();
}

std::size_t ResourceManager::animation_count() const noexcept
{
    return animations_.size();
}

TextureHandle ResourceManager::store_texture(std::string key, Texture texture)
{
    if (!texture.valid()) {
        throw std::invalid_argument("cannot store an invalid texture");
    }
    const auto existing = texture_keys_.find(key);
    if (existing != texture_keys_.end()) {
        return existing->second;
    }
    if (next_texture_id_ == 0U) {
        throw std::overflow_error("texture handle space exhausted");
    }

    const TextureHandle handle{next_texture_id_++};
    textures_.emplace(handle.value, std::move(texture));
    texture_keys_.emplace(std::move(key), handle);
    return handle;
}

} // namespace independent_fighter
