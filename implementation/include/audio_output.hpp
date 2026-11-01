#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <span>
#include <vector>

namespace independent_fighter {

class AudioOutput {
public:
    virtual ~AudioOutput() = default;
    virtual bool initialize(std::uint32_t sample_rate, std::uint16_t channels) = 0;
    virtual void submit(std::span<const std::int16_t> interleaved_samples) = 0;
    virtual void shutdown() noexcept = 0;
};

class NullAudioOutput final : public AudioOutput {
public:
    bool initialize(std::uint32_t sample_rate, std::uint16_t channels) override;
    void submit(std::span<const std::int16_t> interleaved_samples) override;
    void shutdown() noexcept override;

    std::uint64_t submitted_frames() const noexcept;
    std::size_t submitted_blocks() const noexcept;
    std::span<const std::int16_t> last_block() const noexcept;

private:
    std::uint16_t channels_{};
    std::uint64_t submitted_frames_{};
    std::size_t submitted_blocks_{};
    std::vector<std::int16_t> last_block_{};
    bool initialized_{};
};

class WavFileAudioOutput final : public AudioOutput {
public:
    explicit WavFileAudioOutput(std::filesystem::path path);
    ~WavFileAudioOutput() override;

    bool initialize(std::uint32_t sample_rate, std::uint16_t channels) override;
    void submit(std::span<const std::int16_t> interleaved_samples) override;
    void shutdown() noexcept override;

private:
    std::filesystem::path path_{};
    std::ofstream file_{};
    std::uint32_t sample_rate_{};
    std::uint16_t channels_{};
    std::uint32_t data_bytes_{};
    bool initialized_{};
};

} // namespace independent_fighter
