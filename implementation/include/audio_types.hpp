#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace independent_fighter {

struct AudioHandle {
    std::uint32_t value{};

    explicit operator bool() const noexcept
    {
        return value != 0U;
    }

    bool operator==(const AudioHandle&) const = default;
};

enum class AudioCue : std::uint8_t {
    Hit,
    Punch,
    Kick,
    Jump,
    Landing,
    SpecialAttack,
    Block,
    RoundStart,
    RoundWin,
    RoundLoss,
    Victory,
    Defeat
};

enum class AudioContentOrigin : std::uint8_t {
    LoadedPcmFile,
    ProceduralPlaceholder
};

struct AudioClip {
    std::uint32_t sample_rate{};
    std::uint16_t channels{};
    std::vector<std::int16_t> interleaved_samples;
    AudioContentOrigin origin{AudioContentOrigin::LoadedPcmFile};

    std::size_t frame_count() const noexcept;
    bool valid() const noexcept;
};

} // namespace independent_fighter
