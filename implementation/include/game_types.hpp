#pragma once

#include <cstdint>
#include <optional>

namespace independent_fighter {

enum class PlayerId : std::uint8_t {
    One,
    Two
};

enum class CharacterId : std::uint8_t {
    TrainingBalanced,
    TrainingHeavy
};

enum class GameState : std::uint8_t {
    Title,
    CharacterSelect,
    FightIntro,
    Fighting,
    RoundResult,
    Victory,
    Defeat,
    GameOver,
    Paused
};

enum class MovementState : std::uint8_t {
    Standing,
    Walking,
    Running,
    Jumping,
    Falling,
    Crouching,
    Landing
};

enum class ActionState : std::uint8_t {
    Neutral
};

enum class GameplayEventType : std::uint8_t {
    StateChanged,
    CharacterSelected,
    JumpStarted,
    Landed,
    HitConfirmed,
    RoundStarted,
    RoundWon,
    RoundLost,
    GameOver
};

enum class Facing : std::int8_t {
    Left = -1,
    Right = 1
};

struct Vec2 {
    double x{};
    double y{};
};

struct PlayerInput {
    bool left{};
    bool right{};
    bool up{};
    bool down{};
    bool run{};
    bool jump{};
    bool crouch{};
    bool light_punch{};
    bool medium_punch{};
    bool heavy_punch{};
    bool light_kick{};
    bool medium_kick{};
    bool heavy_kick{};
    bool cycle_character{};
    bool confirm{};
};

struct FrameInput {
    PlayerInput player_one{};
    PlayerInput player_two{};
    bool confirm{};
    bool pause{};
    bool restart{};
};

struct PlayerState {
    PlayerId id{PlayerId::One};
    CharacterId character{CharacterId::TrainingBalanced};
    Vec2 position{};
    Vec2 velocity{};
    MovementState movement{MovementState::Standing};
    ActionState action{ActionState::Neutral};
    Facing facing{Facing::Right};
    int health{};
    int rounds_won{};
    bool character_locked{};
    bool jump_was_pressed{};
};

struct GameplayEvent {
    GameplayEventType type{GameplayEventType::StateChanged};
    std::optional<PlayerId> player{};
    GameState state{GameState::Title};
};

} // namespace independent_fighter
