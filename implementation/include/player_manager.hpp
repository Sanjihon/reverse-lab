#pragma once

#include "game_types.hpp"

#include <array>

namespace independent_fighter {

class PlayerManager {
public:
    void initialize() noexcept;
    void prepare_round() noexcept;

    PlayerState& get(PlayerId id) noexcept;
    const PlayerState& get(PlayerId id) const noexcept;

    bool all_characters_locked() const noexcept;
    void face_opponents() noexcept;
    void award_round(PlayerId winner) noexcept;

private:
    static constexpr std::size_t index_of(PlayerId id) noexcept
    {
        return id == PlayerId::One ? 0U : 1U;
    }

    std::array<PlayerState, 2> players_{};
};

} // namespace independent_fighter
