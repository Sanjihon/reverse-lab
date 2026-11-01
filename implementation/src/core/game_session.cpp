#include "game_session.hpp"

#include "character_definition.hpp"
#include "movement_system.hpp"

#include <utility>

namespace independent_fighter {
namespace {

const PlayerInput& input_for(const FrameInput& input, PlayerId id) noexcept
{
    return id == PlayerId::One ? input.player_one : input.player_two;
}

PlayerId opponent_of(PlayerId id) noexcept
{
    return id == PlayerId::One ? PlayerId::Two : PlayerId::One;
}

} // namespace

GameSession::GameSession() noexcept
{
    initialize();
}

void GameSession::initialize() noexcept
{
    pending_events_.clear();
    players_.initialize();
    state_ = GameState::Title;
    resume_state_ = GameState::Fighting;
    round_winner_.reset();
}

void GameSession::restart()
{
    players_.initialize();
    round_winner_.reset();
    transition_to(GameState::Title);
}

std::vector<GameplayEvent> GameSession::drain_events()
{
    std::vector<GameplayEvent> events = std::move(pending_events_);
    pending_events_.clear();
    return events;
}

void GameSession::process_frame(const FrameInput& input)
{
    if (input.restart) {
        restart();
        return;
    }

    if (state_ == GameState::Paused) {
        if (input.pause || input.confirm) {
            transition_to(resume_state_);
        }
        return;
    }

    if (state_ == GameState::Fighting && input.pause) {
        resume_state_ = state_;
        transition_to(GameState::Paused);
        return;
    }

    switch (state_) {
    case GameState::Title:
        if (input.confirm) {
            transition_to(GameState::CharacterSelect);
        }
        break;
    case GameState::CharacterSelect:
        process_character_select(input);
        break;
    case GameState::FightIntro:
        if (input.confirm) {
            transition_to(GameState::Fighting);
        }
        break;
    case GameState::Fighting:
        process_fighting(input);
        break;
    case GameState::RoundResult:
        if (input.confirm) {
            finish_round_result();
        }
        break;
    case GameState::Victory:
    case GameState::Defeat:
        if (input.confirm) {
            transition_to(GameState::GameOver);
        }
        break;
    case GameState::GameOver:
        if (input.confirm) {
            restart();
        }
        break;
    case GameState::Paused:
        break;
    }
}

void GameSession::apply_damage(PlayerId victim, int amount)
{
    if (state_ != GameState::Fighting || amount <= 0 || round_winner_.has_value()) {
        return;
    }

    PlayerState& target = players_.get(victim);
    target.health = amount >= target.health ? 0 : target.health - amount;
    pending_events_.push_back(GameplayEvent{GameplayEventType::HitConfirmed, victim, state_});
    if (target.health == 0) {
        const PlayerId winner = opponent_of(victim);
        players_.award_round(winner);
        round_winner_ = winner;
        pending_events_.push_back(GameplayEvent{GameplayEventType::RoundWon, winner, state_});
        pending_events_.push_back(GameplayEvent{GameplayEventType::RoundLost, victim, state_});
        transition_to(GameState::RoundResult);
    }
}

GameState GameSession::state() const noexcept
{
    return state_;
}

const PlayerState& GameSession::player(PlayerId id) const noexcept
{
    return players_.get(id);
}

std::optional<PlayerId> GameSession::round_winner() const noexcept
{
    return round_winner_;
}

void GameSession::process_character_select(const FrameInput& input)
{
    for (PlayerId id : {PlayerId::One, PlayerId::Two}) {
        PlayerState& selected_player = players_.get(id);
        const PlayerInput& player_input = input_for(input, id);
        if (selected_player.character_locked) {
            continue;
        }

        if (player_input.cycle_character) {
            selected_player.character = next_character(selected_player.character);
            selected_player.health = character_definition(selected_player.character).max_health;
            pending_events_.push_back(GameplayEvent{
                GameplayEventType::CharacterSelected, id, state_});
        }
        if (player_input.confirm) {
            selected_player.character_locked = true;
        }
    }

    if (players_.all_characters_locked()) {
        begin_round();
    }
}

void GameSession::process_fighting(const FrameInput& input)
{
    PlayerState& player_one = players_.get(PlayerId::One);
    PlayerState& player_two = players_.get(PlayerId::Two);
    const MovementState player_one_previous = player_one.movement;
    const MovementState player_two_previous = player_two.movement;
    update_movement(
        player_one,
        input.player_one,
        character_definition(player_one.character));
    update_movement(
        player_two,
        input.player_two,
        character_definition(player_two.character));
    if (player_one_previous != MovementState::Jumping
        && player_one.movement == MovementState::Jumping) {
        pending_events_.push_back(GameplayEvent{
            GameplayEventType::JumpStarted, PlayerId::One, state_});
    }
    if (player_two_previous != MovementState::Jumping
        && player_two.movement == MovementState::Jumping) {
        pending_events_.push_back(GameplayEvent{
            GameplayEventType::JumpStarted, PlayerId::Two, state_});
    }
    if (player_one_previous != MovementState::Landing
        && player_one.movement == MovementState::Landing) {
        pending_events_.push_back(GameplayEvent{GameplayEventType::Landed, PlayerId::One, state_});
    }
    if (player_two_previous != MovementState::Landing
        && player_two.movement == MovementState::Landing) {
        pending_events_.push_back(GameplayEvent{GameplayEventType::Landed, PlayerId::Two, state_});
    }
    players_.face_opponents();
}

void GameSession::begin_round()
{
    players_.prepare_round();
    round_winner_.reset();
    transition_to(GameState::FightIntro);
    pending_events_.push_back(GameplayEvent{GameplayEventType::RoundStarted, std::nullopt, state_});
}

void GameSession::finish_round_result()
{
    if (!round_winner_.has_value()) {
        begin_round();
        return;
    }

    if (players_.get(*round_winner_).rounds_won >= rounds_to_win) {
        transition_to(*round_winner_ == PlayerId::One
            ? GameState::Victory
            : GameState::Defeat);
        return;
    }

    begin_round();
}

void GameSession::transition_to(GameState next)
{
    if (state_ == next) {
        return;
    }
    state_ = next;
    pending_events_.push_back(GameplayEvent{GameplayEventType::StateChanged, std::nullopt, state_});
    if (state_ == GameState::GameOver) {
        pending_events_.push_back(GameplayEvent{GameplayEventType::GameOver, std::nullopt, state_});
    }
}

} // namespace independent_fighter
