#pragma once

#include "character_definition.hpp"
#include "game_types.hpp"

namespace independent_fighter {

void update_movement(
    PlayerState& player,
    const PlayerInput& input,
    const CharacterDefinition& character);

} // namespace independent_fighter
