#pragma once

#include "audio_types.hpp"

#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>

namespace independent_fighter {

class AudioResourceManager {
public:
    AudioHandle load_wav_pcm16(const std::filesystem::path& path);
    AudioHandle create_tone(
        std::string key,
        std::uint32_t sample_rate,
        double frequency_hz,
        std::uint32_t duration_ms);
    AudioHandle create_music_placeholder(
        std::string key,
        std::uint32_t sample_rate,
        bool alternate_phrase = false);

    const AudioClip* clip(AudioHandle handle) const noexcept;
    bool unload(AudioHandle handle) noexcept;
    void clear() noexcept;
    std::size_t size() const noexcept;

private:
    AudioHandle store(std::string key, AudioClip clip);

    std::uint32_t next_handle_{1U};
    std::unordered_map<std::uint32_t, AudioClip> clips_{};
    std::unordered_map<std::string, AudioHandle> keys_{};
};

} // namespace independent_fighter
