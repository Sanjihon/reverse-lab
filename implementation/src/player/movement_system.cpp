#include "movement_system.hpp"

#include <algorithm>

namespace independent_fighter {
namespace {

int horizontal_direction(const PlayerInput& input) noexcept
{
    if (input.left == input.right) {
        return 0;
    }
    return input.right ? 1 : -1;
}

} // namespace

void update_movement(
    PlayerState& player,
    const PlayerInput& input,
    const CharacterDefinition& character)
{
    const int direction = horizontal_direction(input);
    const bool jump_started = input.jump && !player.jump_was_pressed;
    player.jump_was_pressed = input.jump;

    const double horizontal_speed = input.run
        ? character.movement.run_speed
        : character.movement.walk_speed;
    player.velocity.x = static_cast<double>(direction) * horizontal_speed;
    player.position.x += player.velocity.x;

    const bool was_airborne = player.movement == MovementState::Jumping
        || player.movement == MovementState::Falling
        || player.position.y < character.ground_y;

    if (!was_airborne) {
        if (input.crouch) {
            player.velocity.y = 0.0;
            player.movement = MovementState::Crouching;
        } else if (jump_started) {
            player.velocity.y = character.movement.jump_velocity;
            player.movement = MovementState::Jumping;
        } else if (direction != 0) {
            player.movement = input.run ? MovementState::Running : MovementState::Walking;
            player.velocity.y = 0.0;
        } else {
            player.movement = MovementState::Standing;
            player.velocity.y = 0.0;
        }
    }

    const bool airborne = was_airborne || player.movement == MovementState::Jumping;
    if (airborne) {
        player.velocity.y += character.movement.gravity_per_tick;
        player.position.y += player.velocity.y;
        player.movement = player.velocity.y < 0.0
            ? MovementState::Jumping
            : MovementState::Falling;

        if (player.position.y >= character.ground_y) {
            player.position.y = character.ground_y;
            player.velocity.y = 0.0;
            player.movement = MovementState::Landing;
        }
    }

    player.position.x = std::clamp(
        player.position.x,
        character.stage_min_x,
        character.stage_max_x);
}

} // namespace independent_fighter
