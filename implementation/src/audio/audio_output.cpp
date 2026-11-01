#include "audio_output.hpp"

#include <bit>
#include <limits>
#include <stdexcept>
#include <utility>

namespace independent_fighter {
namespace {

void write_u16_le(std::ostream& output, std::uint16_t value)
{
    const char bytes[]{static_cast<char>(value & 0xffU),
        static_cast<char>((value >> 8U) & 0xffU)};
    output.write(bytes, 2);
}

void write_u32_le(std::ostream& output, std::uint32_t value)
{
    const char bytes[]{
        static_cast<char>(value & 0xffU),
        static_cast<char>((value >> 8U) & 0xffU),
        static_cast<char>((value >> 16U) & 0xffU),
        static_cast<char>((value >> 24U) & 0xffU)
    };
    output.write(bytes, 4);
}

} // namespace

bool NullAudioOutput::initialize(std::uint32_t sample_rate, std::uint16_t channels)
{
    initialized_ = sample_rate > 0U && (channels == 1U || channels == 2U);
    channels_ = initialized_ ? channels : 0U;
    return initialized_;
}

void NullAudioOutput::submit(std::span<const std::int16_t> interleaved_samples)
{
    if (!initialized_ || channels_ == 0U) {
        return;
    }
    last_block_.assign(interleaved_samples.begin(), interleaved_samples.end());
    submitted_frames_ += interleaved_samples.size() / channels_;
    ++submitted_blocks_;
}

void NullAudioOutput::shutdown() noexcept
{
    initialized_ = false;
    channels_ = 0U;
    last_block_.clear();
}

std::uint64_t NullAudioOutput::submitted_frames() const noexcept
{
    return submitted_frames_;
}

std::size_t NullAudioOutput::submitted_blocks() const noexcept
{
    return submitted_blocks_;
}

std::span<const std::int16_t> NullAudioOutput::last_block() const noexcept
{
    return last_block_;
}

WavFileAudioOutput::WavFileAudioOutput(std::filesystem::path path)
    : path_(std::move(path))
{
}

WavFileAudioOutput::~WavFileAudioOutput()
{
    shutdown();
}

bool WavFileAudioOutput::initialize(std::uint32_t sample_rate, std::uint16_t channels)
{
    if (initialized_) {
        shutdown();
    }
    if (sample_rate == 0U || (channels != 1U && channels != 2U)
        || sample_rate > std::numeric_limits<std::uint32_t>::max() / (channels * 2U)) {
        return false;
    }

    file_.open(path_, std::ios::binary | std::ios::trunc);
    if (!file_) {
        return false;
    }
    sample_rate_ = sample_rate;
    channels_ = channels;
    data_bytes_ = 0U;
    file_.write("RIFF", 4);
    write_u32_le(file_, 36U);
    file_.write("WAVEfmt ", 8);
    write_u32_le(file_, 16U);
    write_u16_le(file_, 1U);
    write_u16_le(file_, channels_);
    write_u32_le(file_, sample_rate_);
    write_u32_le(file_, sample_rate_ * channels_ * 2U);
    write_u16_le(file_, static_cast<std::uint16_t>(channels_ * 2U));
    write_u16_le(file_, 16U);
    file_.write("data", 4);
    write_u32_le(file_, 0U);
    initialized_ = file_.good();
    return initialized_;
}

void WavFileAudioOutput::submit(std::span<const std::int16_t> interleaved_samples)
{
    if (!initialized_ || channels_ == 0U) {
        throw std::logic_error("WAV output is not initialized");
    }
    if (interleaved_samples.size() % channels_ != 0U) {
        throw std::invalid_argument("audio block is not channel-aligned");
    }
    const std::uint64_t additional_bytes = static_cast<std::uint64_t>(interleaved_samples.size()) * 2U;
    if (additional_bytes > std::numeric_limits<std::uint32_t>::max() - 36U - data_bytes_) {
        throw std::length_error("WAV output exceeds RIFF size limit");
    }

    for (const std::int16_t sample : interleaved_samples) {
        write_u16_le(file_, std::bit_cast<std::uint16_t>(sample));
    }
    if (!file_) {
        throw std::runtime_error("failed writing WAV audio block");
    }
    data_bytes_ += static_cast<std::uint32_t>(additional_bytes);
}

void WavFileAudioOutput::shutdown() noexcept
{
    if (!file_.is_open()) {
        initialized_ = false;
        return;
    }
    if (initialized_) {
        file_.seekp(4, std::ios::beg);
        write_u32_le(file_, 36U + data_bytes_);
        file_.seekp(40, std::ios::beg);
        write_u32_le(file_, data_bytes_);
        file_.flush();
    }
    file_.close();
    initialized_ = false;
    channels_ = 0U;
}

} // namespace independent_fighter
