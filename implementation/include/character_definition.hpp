#pragma once

#include "game_types.hpp"

#include <string_view>

namespace independent_fighter {

struct MovementTuning {
    double walk_speed;
    double run_speed;
    double jump_velocity;
    double gravity_per_tick;
};

struct CharacterDefinition {
    CharacterId id;
    std::string_view label;
    int max_health;
    MovementTuning movement;
    double ground_y;
    double stage_min_x;
    double stage_max_x;
    double player_one_start_x;
    double player_two_start_x;
};

const CharacterDefinition& character_definition(CharacterId id) noexcept;
CharacterId next_character(CharacterId id) noexcept;

} // namespace independent_fighter
