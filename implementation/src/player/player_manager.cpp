#include "player_manager.hpp"

#include "character_definition.hpp"

namespace independent_fighter {

void PlayerManager::initialize() noexcept
{
    players_ = {};
    players_[0].id = PlayerId::One;
    players_[0].character = CharacterId::TrainingBalanced;
    players_[1].id = PlayerId::Two;
    players_[1].character = CharacterId::TrainingHeavy;
    prepare_round();
}

void PlayerManager::prepare_round() noexcept
{
    for (PlayerState& player : players_) {
        const CharacterDefinition& definition = character_definition(player.character);
        const bool is_player_one = player.id == PlayerId::One;
        player.position = Vec2{
            is_player_one ? definition.player_one_start_x : definition.player_two_start_x,
            definition.ground_y
        };
        player.velocity = {};
        player.movement = MovementState::Standing;
        player.action = ActionState::Neutral;
        player.facing = is_player_one ? Facing::Right : Facing::Left;
        player.health = definition.max_health;
        player.jump_was_pressed = false;
    }
}

PlayerState& PlayerManager::get(PlayerId id) noexcept
{
    return players_[index_of(id)];
}

const PlayerState& PlayerManager::get(PlayerId id) const noexcept
{
    return players_[index_of(id)];
}

bool PlayerManager::all_characters_locked() const noexcept
{
    return players_[0].character_locked && players_[1].character_locked;
}

void PlayerManager::face_opponents() noexcept
{
    if (players_[0].position.x < players_[1].position.x) {
        players_[0].facing = Facing::Right;
        players_[1].facing = Facing::Left;
    } else if (players_[0].position.x > players_[1].position.x) {
        players_[0].facing = Facing::Left;
        players_[1].facing = Facing::Right;
    }
}

void PlayerManager::award_round(PlayerId winner) noexcept
{
    ++get(winner).rounds_won;
}

} // namespace independent_fighter
