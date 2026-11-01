#include "character_definition.hpp"

namespace independent_fighter {
namespace {

// New prototype parameters; these are not verified Street Fighter II values.
constexpr CharacterDefinition balanced_training{
    CharacterId::TrainingBalanced,
    "Training Balanced",
    1000,
    MovementTuning{2.0, 3.5, -8.0, 0.45},
    300.0,
    20.0,
    620.0,
    160.0,
    480.0
};

constexpr CharacterDefinition heavy_training{
    CharacterId::TrainingHeavy,
    "Training Heavy",
    1100,
    MovementTuning{1.7, 2.8, -7.2, 0.48},
    300.0,
    20.0,
    620.0,
    160.0,
    480.0
};

} // namespace

const CharacterDefinition& character_definition(CharacterId id) noexcept
{
    switch (id) {
    case CharacterId::TrainingBalanced:
        return balanced_training;
    case CharacterId::TrainingHeavy:
        return heavy_training;
    }
    return balanced_training;
}

CharacterId next_character(CharacterId id) noexcept
{
    return id == CharacterId::TrainingBalanced
        ? CharacterId::TrainingHeavy
        : CharacterId::TrainingBalanced;
}

} // namespace independent_fighter
