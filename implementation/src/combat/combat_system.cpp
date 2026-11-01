#include "combat_system.hpp"

#include <array>
#include <cmath>

namespace independent_fighter {
namespace {

struct AttackDefinition {
    AttackKind kind;
    int CombatConfig::*damage_member;
    bool PlayerInput::*input_member;
};

constexpr std::array<AttackDefinition, 6> attack_priority{{
    {AttackKind::HeavyPunch, &CombatConfig::heavy_punch_damage, &PlayerInput::heavy_punch},
    {AttackKind::HeavyKick, &CombatConfig::heavy_kick_damage, &PlayerInput::heavy_kick},
    {AttackKind::MediumPunch, &CombatConfig::medium_punch_damage, &PlayerInput::medium_punch},
    {AttackKind::MediumKick, &CombatConfig::medium_kick_damage, &PlayerInput::medium_kick},
    {AttackKind::LightPunch, &CombatConfig::light_punch_damage, &PlayerInput::light_punch},
    {AttackKind::LightKick, &CombatConfig::light_kick_damage, &PlayerInput::light_kick}
}};

const PlayerInput& input_for(const FrameInput& input, PlayerId player) noexcept
{
    return player == PlayerId::One ? input.player_one : input.player_two;
}

PlayerId opponent(PlayerId player) noexcept
{
    return player == PlayerId::One ? PlayerId::Two : PlayerId::One;
}

int attack_damage(const CombatConfig& config, const PlayerInput& input, AttackKind& kind) noexcept
{
    for (const AttackDefinition& definition : attack_priority) {
        if (input.*(definition.input_member)) {
            kind = definition.kind;
            return config.*(definition.damage_member);
        }
    }
    return 0;
}

} // namespace

CombatSystem::CombatSystem(CombatConfig config)
    : config_(config)
{
    valid_ = std::isfinite(config_.attack_reach)
        && std::isfinite(config_.attack_height)
        && config_.attack_reach > 0.0
        && config_.attack_height > 0.0
        && config_.light_punch_damage >= 0
        && config_.medium_punch_damage >= 0
        && config_.heavy_punch_damage >= 0
        && config_.light_kick_damage >= 0
        && config_.medium_kick_damage >= 0
        && config_.heavy_kick_damage >= 0;
}

std::vector<CombatHit> CombatSystem::resolve(
    GameSession& session,
    const FrameInput& input,
    std::uint64_t tick)
{
    std::vector<CombatHit> hits;
    if (!valid_ || session.state() != GameState::Fighting) {
        return hits;
    }

    for (const PlayerId attacker : {PlayerId::One, PlayerId::Two}) {
        const std::size_t attacker_index = attacker == PlayerId::One ? 0U : 1U;
        if (tick < next_attack_tick_[attacker_index]) {
            continue;
        }

        AttackKind kind{};
        const int damage = attack_damage(config_, input_for(input, attacker), kind);
        if (damage <= 0) {
            continue;
        }
        next_attack_tick_[attacker_index] = tick + config_.cooldown_ticks;

        const PlayerState& attacking_player = session.player(attacker);
        const PlayerState& defending_player = session.player(opponent(attacker));
        if (!CollisionSystem::overlaps(
                collision_.attackbox(attacking_player,
                    config_.attack_reach, config_.attack_height),
                collision_.hurtbox(defending_player))) {
            continue;
        }

        hits.push_back(CombatHit{attacker, opponent(attacker), kind, damage});
        break;
    }
    return hits;
}

const CombatConfig& CombatSystem::config() const noexcept
{
    return config_;
}

void CombatSystem::reset() noexcept
{
    next_attack_tick_[0] = 0U;
    next_attack_tick_[1] = 0U;
}

bool CombatSystem::valid() const noexcept
{
    return valid_;
}

} // namespace independent_fighter
