#include "collision_system.hpp"

#include <algorithm>

namespace independent_fighter {

bool CollisionSystem::overlaps(const WorldRect& left, const WorldRect& right) noexcept
{
    if (left.right <= left.left || left.bottom <= left.top
        || right.right <= right.left || right.bottom <= right.top) {
        return false;
    }
    return left.left < right.right && right.left < left.right
        && left.top < right.bottom && right.top < left.bottom;
}

WorldRect CollisionSystem::hurtbox(const PlayerState& player) const noexcept
{
    return WorldRect{
        player.position.x - hurtbox_half_width,
        player.position.y - hurtbox_height,
        player.position.x + hurtbox_half_width,
        player.position.y
    };
}

WorldRect CollisionSystem::attackbox(
    const PlayerState& player,
    double reach,
    double height) const noexcept
{
    const double forward_start = static_cast<int>(player.facing) > 0
        ? player.position.x + attack_origin_offset
        : player.position.x - attack_origin_offset;
    const double other_edge = forward_start + static_cast<int>(player.facing) * reach;
    return WorldRect{
        std::min(forward_start, other_edge),
        player.position.y - height,
        std::max(forward_start, other_edge),
        player.position.y
    };
}

} // namespace independent_fighter
