#include "audio_resource_manager.hpp"

#include <algorithm>
#include <bit>
#include <cmath>
#include <fstream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace independent_fighter {
namespace {

bool has_tag(const std::vector<std::uint8_t>& bytes, std::size_t offset, const char* tag)
{
    return offset <= bytes.size() && bytes.size() - offset >= 4U
        && bytes[offset] == static_cast<std::uint8_t>(tag[0])
        && bytes[offset + 1U] == static_cast<std::uint8_t>(tag[1])
        && bytes[offset + 2U] == static_cast<std::uint8_t>(tag[2])
        && bytes[offset + 3U] == static_cast<std::uint8_t>(tag[3]);
}

std::uint16_t read_u16_le(const std::vector<std::uint8_t>& bytes, std::size_t offset)
{
    if (offset > bytes.size() || bytes.size() - offset < 2U) {
        throw std::runtime_error("truncated WAV integer");
    }
    return static_cast<std::uint16_t>(bytes[offset])
        | static_cast<std::uint16_t>(static_cast<std::uint16_t>(bytes[offset + 1U]) << 8U);
}

std::uint32_t read_u32_le(const std::vector<std::uint8_t>& bytes, std::size_t offset)
{
    if (offset > bytes.size() || bytes.size() - offset < 4U) {
        throw std::runtime_error("truncated WAV integer");
    }
    return static_cast<std::uint32_t>(bytes[offset])
        | (static_cast<std::uint32_t>(bytes[offset + 1U]) << 8U)
        | (static_cast<std::uint32_t>(bytes[offset + 2U]) << 16U)
        | (static_cast<std::uint32_t>(bytes[offset + 3U]) << 24U);
}

AudioClip make_tone(
    std::uint32_t sample_rate,
    double frequency_hz,
    std::uint32_t duration_ms)
{
    if (sample_rate == 0U || duration_ms == 0U || frequency_hz <= 0.0
        || frequency_hz >= static_cast<double>(sample_rate) / 2.0) {
        throw std::invalid_argument("invalid procedural tone parameters");
    }
    const std::uint64_t sample_count = static_cast<std::uint64_t>(sample_rate)
        * static_cast<std::uint64_t>(duration_ms) / 1000U;
    if (sample_count == 0U || sample_count > static_cast<std::uint64_t>(
        std::numeric_limits<std::size_t>::max())) {
        throw std::invalid_argument("procedural tone duration is not representable");
    }

    AudioClip clip{};
    clip.sample_rate = sample_rate;
    clip.channels = 1U;
    clip.origin = AudioContentOrigin::ProceduralPlaceholder;
    clip.interleaved_samples.resize(static_cast<std::size_t>(sample_count));
    constexpr double pi = 3.14159265358979323846;
    const double attack_samples = std::max(1.0, static_cast<double>(sample_rate) * 0.008);
    const double release_samples = std::max(1.0, static_cast<double>(sample_rate) * 0.025);
    for (std::size_t index = 0; index < clip.interleaved_samples.size(); ++index) {
        const double attack = std::min(1.0, static_cast<double>(index) / attack_samples);
        const double remaining = static_cast<double>(clip.interleaved_samples.size() - index);
        const double release = std::min(1.0, remaining / release_samples);
        const double phase = 2.0 * pi * frequency_hz * static_cast<double>(index)
            / static_cast<double>(sample_rate);
        const double value = std::sin(phase) * std::min(attack, release) * 0.28;
        clip.interleaved_samples[index] = static_cast<std::int16_t>(
            std::lround(value * static_cast<double>(std::numeric_limits<std::int16_t>::max())));
    }
    return clip;
}

} // namespace

AudioHandle AudioResourceManager::load_wav_pcm16(const std::filesystem::path& path)
{
    const std::string key = std::filesystem::absolute(path).lexically_normal().string();
    const auto cached = keys_.find(key);
    if (cached != keys_.end()) {
        return cached->second;
    }

    std::ifstream input(path, std::ios::binary);
    if (!input) {
        throw std::runtime_error("unable to open WAV file: " + path.string());
    }
    const std::vector<char> raw((std::istreambuf_iterator<char>(input)),
        std::istreambuf_iterator<char>());
    std::vector<std::uint8_t> bytes;
    bytes.reserve(raw.size());
    for (const char value : raw) {
        bytes.push_back(static_cast<std::uint8_t>(static_cast<unsigned char>(value)));
    }
    if (bytes.size() < 12U || !has_tag(bytes, 0U, "RIFF") || !has_tag(bytes, 8U, "WAVE")) {
        throw std::runtime_error("WAV file has no RIFF/WAVE header");
    }

    const std::uint32_t riff_size = read_u32_le(bytes, 4U);
    if (riff_size > bytes.size() - 8U) {
        throw std::runtime_error("WAV RIFF size exceeds file length");
    }
    const std::size_t riff_end = 8U + static_cast<std::size_t>(riff_size);
    std::size_t cursor = 12U;
    bool format_found = false;
    bool data_found = false;
    std::uint16_t format_tag = 0U;
    std::uint16_t channels = 0U;
    std::uint16_t bits_per_sample = 0U;
    std::uint16_t block_align = 0U;
    std::uint32_t sample_rate = 0U;
    std::size_t data_offset = 0U;
    std::size_t data_size = 0U;

    while (cursor <= riff_end && riff_end - cursor >= 8U) {
        const std::uint32_t chunk_size_u32 = read_u32_le(bytes, cursor + 4U);
        const std::size_t payload = cursor + 8U;
        const std::size_t chunk_size = static_cast<std::size_t>(chunk_size_u32);
        if (payload > riff_end || chunk_size > riff_end - payload) {
            throw std::runtime_error("WAV chunk exceeds RIFF bounds");
        }
        if (has_tag(bytes, cursor, "fmt ")) {
            if (chunk_size < 16U) {
                throw std::runtime_error("WAV format chunk is too short");
            }
            format_tag = read_u16_le(bytes, payload);
            channels = read_u16_le(bytes, payload + 2U);
            sample_rate = read_u32_le(bytes, payload + 4U);
            block_align = read_u16_le(bytes, payload + 12U);
            bits_per_sample = read_u16_le(bytes, payload + 14U);
            format_found = true;
        } else if (has_tag(bytes, cursor, "data") && !data_found) {
            data_offset = payload;
            data_size = chunk_size;
            data_found = true;
        }
        const std::size_t padded_size = chunk_size + (chunk_size & 1U);
        if (padded_size > riff_end - payload) {
            break;
        }
        cursor = payload + padded_size;
    }

    if (!format_found || !data_found || format_tag != 1U || sample_rate == 0U
        || (channels != 1U && channels != 2U) || bits_per_sample != 16U
        || block_align != channels * 2U || data_size == 0U
        || data_size % block_align != 0U) {
        throw std::runtime_error("WAV must contain PCM16 mono or stereo audio");
    }

    AudioClip clip{};
    clip.sample_rate = sample_rate;
    clip.channels = channels;
    clip.origin = AudioContentOrigin::LoadedPcmFile;
    clip.interleaved_samples.resize(data_size / 2U);
    for (std::size_t index = 0; index < clip.interleaved_samples.size(); ++index) {
        const std::uint16_t bits = read_u16_le(bytes, data_offset + index * 2U);
        clip.interleaved_samples[index] = std::bit_cast<std::int16_t>(bits);
    }
    return store(key, std::move(clip));
}

AudioHandle AudioResourceManager::create_tone(
    std::string key,
    std::uint32_t sample_rate,
    double frequency_hz,
    std::uint32_t duration_ms)
{
    const std::string resource_key = "procedural:sfx:" + key;
    const auto existing = keys_.find(resource_key);
    if (existing != keys_.end()) {
        return existing->second;
    }
    return store(resource_key, make_tone(sample_rate, frequency_hz, duration_ms));
}

AudioHandle AudioResourceManager::create_music_placeholder(
    std::string key,
    std::uint32_t sample_rate,
    bool alternate_phrase)
{
    const std::string resource_key = "procedural:music:" + key;
    const auto existing = keys_.find(resource_key);
    if (existing != keys_.end()) {
        return existing->second;
    }
    if (sample_rate == 0U) {
        throw std::invalid_argument("music sample rate must be positive");
    }

    constexpr double pi = 3.14159265358979323846;
    const std::uint64_t frames_u64 = static_cast<std::uint64_t>(sample_rate) * 2U;
    if (frames_u64 > static_cast<std::uint64_t>(std::numeric_limits<std::size_t>::max())) {
        throw std::invalid_argument("music duration is not representable");
    }
    const std::size_t frames = static_cast<std::size_t>(frames_u64);
    constexpr double major_notes[]{261.63, 329.63, 392.00, 329.63,
        293.66, 349.23, 440.00, 349.23};
    constexpr double minor_notes[]{220.00, 261.63, 329.63, 261.63,
        196.00, 246.94, 293.66, 246.94};
    const double* notes = alternate_phrase ? minor_notes : major_notes;

    AudioClip clip{};
    clip.sample_rate = sample_rate;
    clip.channels = 1U;
    clip.origin = AudioContentOrigin::ProceduralPlaceholder;
    clip.interleaved_samples.resize(frames);
    const std::size_t frames_per_note = std::max<std::size_t>(1U, frames / 8U);
    for (std::size_t index = 0; index < frames; ++index) {
        const std::size_t note_index = std::min<std::size_t>(7U, index / frames_per_note);
        const double phase = 2.0 * pi * notes[note_index] * static_cast<double>(index)
            / static_cast<double>(sample_rate);
        const double harmonic = std::sin(phase) + 0.2 * std::sin(phase * 2.0);
        const double value = harmonic * 0.10;
        clip.interleaved_samples[index] = static_cast<std::int16_t>(
            std::lround(value * static_cast<double>(std::numeric_limits<std::int16_t>::max())));
    }
    return store(resource_key, std::move(clip));
}

const AudioClip* AudioResourceManager::clip(AudioHandle handle) const noexcept
{
    const auto found = clips_.find(handle.value);
    return found == clips_.end() ? nullptr : &found->second;
}

bool AudioResourceManager::unload(AudioHandle handle) noexcept
{
    const auto found = clips_.find(handle.value);
    if (found == clips_.end()) {
        return false;
    }
    for (auto key = keys_.begin(); key != keys_.end();) {
        if (key->second == handle) {
            key = keys_.erase(key);
        } else {
            ++key;
        }
    }
    clips_.erase(found);
    return true;
}

void AudioResourceManager::clear() noexcept
{
    keys_.clear();
    clips_.clear();
}

std::size_t AudioResourceManager::size() const noexcept
{
    return clips_.size();
}

AudioHandle AudioResourceManager::store(std::string key, AudioClip clip)
{
    if (!clip.valid()) {
        throw std::invalid_argument("cannot store an invalid audio clip");
    }
    const auto existing = keys_.find(key);
    if (existing != keys_.end()) {
        return existing->second;
    }
    if (next_handle_ == 0U) {
        throw std::overflow_error("audio handle space exhausted");
    }

    const AudioHandle handle{next_handle_++};
    clips_.emplace(handle.value, std::move(clip));
    keys_.emplace(std::move(key), handle);
    return handle;
}

std::size_t AudioClip::frame_count() const noexcept
{
    return channels == 0U ? 0U : interleaved_samples.size() / channels;
}

bool AudioClip::valid() const noexcept
{
    return sample_rate > 0U && (channels == 1U || channels == 2U)
        && !interleaved_samples.empty()
        && interleaved_samples.size() % channels == 0U;
}

} // namespace independent_fighter
