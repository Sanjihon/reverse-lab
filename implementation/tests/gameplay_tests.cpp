#include "character_definition.hpp"
#include "game_loop.hpp"
#include "game_session.hpp"
#include "movement_system.hpp"

#include <cmath>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string_view>

namespace {

using namespace independent_fighter;

#define CHECK(expression) \
    do { \
        if (!(expression)) { \
            throw std::runtime_error("check failed: " #expression); \
        } \
    } while (false)

void enter_fighting(GameSession& session)
{
    FrameInput input{};
    input.confirm = true;
    session.process_frame(input);
    CHECK(session.state() == GameState::CharacterSelect);

    input = {};
    input.player_one.confirm = true;
    input.player_two.confirm = true;
    session.process_frame(input);
    CHECK(session.state() == GameState::FightIntro);

    input = {};
    input.confirm = true;
    session.process_frame(input);
    CHECK(session.state() == GameState::Fighting);
}

void test_player_initialization()
{
    const GameSession session{};
    CHECK(session.state() == GameState::Title);
    CHECK(session.player(PlayerId::One).id == PlayerId::One);
    CHECK(session.player(PlayerId::Two).id == PlayerId::Two);
    CHECK(session.player(PlayerId::One).character == CharacterId::TrainingBalanced);
    CHECK(session.player(PlayerId::Two).character == CharacterId::TrainingHeavy);
    CHECK(session.player(PlayerId::One).health == 1000);
    CHECK(session.player(PlayerId::Two).health == 1100);
    CHECK(session.player(PlayerId::One).position.x == 160.0);
    CHECK(session.player(PlayerId::Two).position.x == 480.0);
    CHECK(session.player(PlayerId::One).action == ActionState::Neutral);
    CHECK(session.player(PlayerId::Two).action == ActionState::Neutral);
    CHECK(session.player(PlayerId::One).facing == Facing::Right);
    CHECK(session.player(PlayerId::Two).facing == Facing::Left);
}

void test_title_and_character_select_states()
{
    GameSession session{};
    FrameInput input{};
    input.confirm = true;
    session.process_frame(input);
    CHECK(session.state() == GameState::CharacterSelect);

    input = {};
    input.player_one.confirm = true;
    session.process_frame(input);
    CHECK(session.state() == GameState::CharacterSelect);
    CHECK(!session.player(PlayerId::Two).character_locked);

    input = {};
    input.player_two.confirm = true;
    session.process_frame(input);
    CHECK(session.state() == GameState::FightIntro);
}

void test_character_selection_and_round_reset()
{
    GameSession session{};
    FrameInput input{};
    input.confirm = true;
    session.process_frame(input);

    input = {};
    input.player_one.cycle_character = true;
    input.player_one.confirm = true;
    input.player_two.cycle_character = true;
    input.player_two.confirm = true;
    session.process_frame(input);

    CHECK(session.state() == GameState::FightIntro);
    CHECK(session.player(PlayerId::One).character == CharacterId::TrainingHeavy);
    CHECK(session.player(PlayerId::Two).character == CharacterId::TrainingBalanced);
    CHECK(session.player(PlayerId::One).health == 1100);
    CHECK(session.player(PlayerId::Two).health == 1000);
}

void test_fixed_step_loop()
{
    GameLoop loop{};
    const FrameAdvance advance = loop.advance(GameLoop::fixed_step_seconds * 2.5);
    CHECK(advance.simulation_steps == 2U);
    CHECK(std::abs(advance.interpolation_alpha - 0.5) < 1.0e-9);
    CHECK(advance.dropped_seconds == 0.0);

    loop.reset();
    const FrameAdvance catch_up = loop.advance(1.0);
    CHECK(catch_up.simulation_steps == GameLoop::max_steps_per_advance);
    CHECK(catch_up.dropped_seconds > 0.0);
    CHECK(catch_up.interpolation_alpha >= 0.0);
    CHECK(catch_up.interpolation_alpha < 1.0);
}

void test_walk_and_run_positions()
{
    PlayerState player{};
    player.position = Vec2{100.0, 300.0};
    const CharacterDefinition& character = character_definition(CharacterId::TrainingBalanced);
    PlayerInput input{};
    input.left = true;
    update_movement(player, input, character);
    CHECK(player.position.x == 98.0);
    CHECK(player.movement == MovementState::Walking);

    input = {};
    input.right = true;
    input.run = true;
    update_movement(player, input, character);
    CHECK(player.position.x == 101.5);
    CHECK(player.movement == MovementState::Running);
}

void test_crouch_and_standing()
{
    PlayerState player{};
    player.position = Vec2{100.0, 300.0};
    const CharacterDefinition& character = character_definition(CharacterId::TrainingBalanced);
    PlayerInput input{};
    input.crouch = true;
    update_movement(player, input, character);
    CHECK(player.position.x == 100.0);
    CHECK(player.movement == MovementState::Crouching);

    input = {};
    update_movement(player, input, character);
    CHECK(player.movement == MovementState::Standing);
}

void test_jump_and_land_transitions()
{
    PlayerState player{};
    player.position = Vec2{100.0, 300.0};
    const CharacterDefinition& character = character_definition(CharacterId::TrainingBalanced);
    PlayerInput input{};
    input.jump = true;
    update_movement(player, input, character);
    CHECK(player.movement == MovementState::Jumping);
    CHECK(player.position.y < character.ground_y);

    input = {};
    for (int tick = 0; tick < 120 && player.movement != MovementState::Landing; ++tick) {
        update_movement(player, input, character);
    }
    CHECK(player.movement == MovementState::Landing);
    CHECK(player.position.y == character.ground_y);

    update_movement(player, input, character);
    CHECK(player.movement == MovementState::Standing);
}

void test_stage_position_bounds()
{
    PlayerState player{};
    player.position = Vec2{21.0, 300.0};
    const CharacterDefinition& character = character_definition(CharacterId::TrainingBalanced);
    PlayerInput input{};
    input.left = true;
    update_movement(player, input, character);
    CHECK(player.position.x == character.stage_min_x);

    player.position.x = 619.0;
    input = {};
    input.right = true;
    update_movement(player, input, character);
    CHECK(player.position.x == character.stage_max_x);
}

void test_opponent_facing()
{
    GameSession session{};
    enter_fighting(session);
    FrameInput input{};
    input.player_one.right = true;
    for (int tick = 0; tick < 180; ++tick) {
        session.process_frame(input);
    }
    CHECK(session.player(PlayerId::One).facing == Facing::Left);
    CHECK(session.player(PlayerId::Two).facing == Facing::Right);
}

void test_pause_and_resume()
{
    GameSession session{};
    enter_fighting(session);
    FrameInput input{};
    input.pause = true;
    session.process_frame(input);
    CHECK(session.state() == GameState::Paused);

    const double paused_x = session.player(PlayerId::One).position.x;
    input = {};
    input.player_one.right = true;
    session.process_frame(input);
    CHECK(session.state() == GameState::Paused);
    CHECK(session.player(PlayerId::One).position.x == paused_x);

    input = {};
    input.pause = true;
    session.process_frame(input);
    CHECK(session.state() == GameState::Fighting);
}

void test_round_winner_and_health_reset()
{
    GameSession session{};
    enter_fighting(session);
    session.apply_damage(PlayerId::Two, session.player(PlayerId::Two).health);
    CHECK(session.state() == GameState::RoundResult);
    CHECK(session.player(PlayerId::Two).health == 0);
    CHECK(session.player(PlayerId::One).rounds_won == 1);
    CHECK(session.round_winner() == PlayerId::One);

    FrameInput input{};
    input.confirm = true;
    session.process_frame(input);
    CHECK(session.state() == GameState::FightIntro);
    CHECK(session.player(PlayerId::One).health == 1000);
    CHECK(session.player(PlayerId::Two).health == 1100);
    CHECK(session.player(PlayerId::One).rounds_won == 1);
    CHECK(!session.round_winner().has_value());
}

void test_best_of_three_and_terminal_states()
{
    GameSession session{};
    enter_fighting(session);

    for (int round = 0; round < 2; ++round) {
        session.apply_damage(PlayerId::Two, session.player(PlayerId::Two).health);
        CHECK(session.state() == GameState::RoundResult);
        FrameInput input{};
        input.confirm = true;
        session.process_frame(input);
        if (round == 0) {
            CHECK(session.state() == GameState::FightIntro);
            input = {};
            input.confirm = true;
            session.process_frame(input);
            CHECK(session.state() == GameState::Fighting);
        }
    }

    CHECK(session.state() == GameState::Victory);
    FrameInput input{};
    input.confirm = true;
    session.process_frame(input);
    CHECK(session.state() == GameState::GameOver);
    session.process_frame(input);
    CHECK(session.state() == GameState::Title);
    CHECK(session.player(PlayerId::One).rounds_won == 0);
}

void test_defeat_for_player_one()
{
    GameSession session{};
    enter_fighting(session);
    session.apply_damage(PlayerId::One, session.player(PlayerId::One).health);
    FrameInput input{};
    input.confirm = true;
    session.process_frame(input);
    input = {};
    input.confirm = true;
    session.process_frame(input);

    session.apply_damage(PlayerId::One, session.player(PlayerId::One).health);
    input = {};
    input.confirm = true;
    session.process_frame(input);
    CHECK(session.state() == GameState::Defeat);
}

void test_restart_resets_session()
{
    GameSession session{};
    enter_fighting(session);
    session.apply_damage(PlayerId::Two, 100);
    FrameInput input{};
    input.restart = true;
    session.process_frame(input);
    CHECK(session.state() == GameState::Title);
    CHECK(session.player(PlayerId::One).rounds_won == 0);
    CHECK(session.player(PlayerId::Two).health == 1100);
}

struct TestCase {
    std::string_view name;
    void (*run)();
};

} // namespace

int main()
{
    constexpr TestCase tests[]{
        {"player initialization", test_player_initialization},
        {"title and character select", test_title_and_character_select_states},
        {"character selection", test_character_selection_and_round_reset},
        {"fixed-step loop", test_fixed_step_loop},
        {"walk and run positions", test_walk_and_run_positions},
        {"crouch and stand", test_crouch_and_standing},
        {"jump and land", test_jump_and_land_transitions},
        {"stage bounds", test_stage_position_bounds},
        {"opponent facing", test_opponent_facing},
        {"pause and resume", test_pause_and_resume},
        {"round winner and reset", test_round_winner_and_health_reset},
        {"best of three and terminal states", test_best_of_three_and_terminal_states},
        {"defeat state", test_defeat_for_player_one},
        {"restart", test_restart_resets_session}
    };

    int passed = 0;
    for (const TestCase& test : tests) {
        try {
            test.run();
            ++passed;
            std::cout << "PASS " << test.name << '\n';
        } catch (const std::exception& error) {
            std::cerr << "FAIL " << test.name << ": " << error.what() << '\n';
            return 1;
        }
    }

    std::cout << passed << " gameplay tests passed\n";
    return 0;
}
