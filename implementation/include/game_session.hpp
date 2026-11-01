#pragma once

#include "game_types.hpp"
#include "player_manager.hpp"

#include <optional>
#include <vector>

namespace independent_fighter {

class GameSession {
public:
    static constexpr int rounds_to_win = 2;

    GameSession() noexcept;

    void initialize() noexcept;
    void process_frame(const FrameInput& input);
    void apply_damage(PlayerId victim, int amount);
    void restart();
    std::vector<GameplayEvent> drain_events();

    GameState state() const noexcept;
    const PlayerState& player(PlayerId id) const noexcept;
    std::optional<PlayerId> round_winner() const noexcept;

private:
    void process_character_select(const FrameInput& input);
    void process_fighting(const FrameInput& input);
    void begin_round();
    void finish_round_result();
    void transition_to(GameState next);

    PlayerManager players_{};
    GameState state_{GameState::Title};
    GameState resume_state_{GameState::Fighting};
    std::optional<PlayerId> round_winner_{};
    std::vector<GameplayEvent> pending_events_{};
};

} // namespace independent_fighter
