#pragma once

#include "collision_system.hpp"
#include "game_session.hpp"

#include <cstdint>
#include <vector>

namespace independent_fighter {

enum class AttackKind : std::uint8_t {
    LightPunch,
    MediumPunch,
    HeavyPunch,
    LightKick,
    MediumKick,
    HeavyKick
};

struct CombatHit {
    PlayerId attacker{PlayerId::One};
    PlayerId victim{PlayerId::Two};
    AttackKind attack{AttackKind::LightPunch};
    int damage{};
};

struct CombatConfig {
    double attack_reach{52.0};
    double attack_height{58.0};
    std::uint64_t cooldown_ticks{8U};
    int light_punch_damage{70};
    int medium_punch_damage{95};
    int heavy_punch_damage{150};
    int light_kick_damage{65};
    int medium_kick_damage{90};
    int heavy_kick_damage{140};
};

class CombatSystem {
public:
    explicit CombatSystem(CombatConfig config = {});

    std::vector<CombatHit> resolve(
        GameSession& session,
        const FrameInput& input,
        std::uint64_t tick);

    void reset() noexcept;
    const CombatConfig& config() const noexcept;
    bool valid() const noexcept;

private:
    CombatConfig config_{};
    CollisionSystem collision_{};
    std::uint64_t next_attack_tick_[2]{};
    bool valid_{true};
};

} // namespace independent_fighter
