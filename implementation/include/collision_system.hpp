#pragma once

#include "game_types.hpp"

namespace independent_fighter {

struct WorldRect {
    double left{};
    double top{};
    double right{};
    double bottom{};
};

class CollisionSystem {
public:
    static bool overlaps(const WorldRect& left, const WorldRect& right) noexcept;
    WorldRect hurtbox(const PlayerState& player) const noexcept;
    WorldRect attackbox(const PlayerState& player, double reach, double height) const noexcept;

    double hurtbox_half_width{14.0};
    double hurtbox_height{64.0};
    double attack_origin_offset{8.0};
};

} // namespace independent_fighter
