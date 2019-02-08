//
// Created by Alexandra Zaharia on 05/02/19.
//

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>
#include <stdlib.h>

#include "src/galaxy.h"
#include "src/player.h"
#include "src/ai.h"
#include "vector.h"

// Fixtures --------------------------------------------------------------------

// Fixture setup: creates a galaxy and adds two AI players to it
static int setup_galaxy_2_players(void** state)
{
    Galaxy* galaxy = galaxy_create();
    assert_non_null(galaxy);

    Player* ai1 = player_create('@', BOLDRED);
    assert_non_null(ai1);

    Player* ai2 = player_create('%', BOLDGREEN);
    assert_non_null(ai2);

    Vector* players = vector_create();
    assert_non_null(players);
    players->add(players, ai1);
    players->add(players, ai2);

    ai1->play = ai_strategy;
    ai2->play = ai_strategy;

    assert_true(galaxy->initialize(galaxy, players));

    *state = galaxy;
    return 0;
}

// Fixture teardown: frees the Galaxy
static int teardown_galaxy(void** state)
{
    Galaxy* galaxy = (Galaxy*) *state;
    galaxy->destroy(galaxy);
    *state = galaxy;
    return 0;
}


// Tests -----------------------------------------------------------------------

// Tests galaxy initialization for a game with two AI players
static void test_galaxy_init_2_players(void** state)
{
    Galaxy* galaxy = (Galaxy*) *state;
    assert_int_equal((int) galaxy->players->size, 2);

    Player* player1 = galaxy->players->data[0];
    assert_true(player1->symbol == '@');
    assert_string_equal(player1->color, BOLDRED);

    Player* player2 = galaxy->players->data[1];
    assert_true(player2->symbol == '%');
    assert_string_equal(player2->color, BOLDGREEN);

    assert_true(player1->home_planet->x != player2->home_planet->x
             || player1->home_planet->y != player2->home_planet->y);

    *state = galaxy;
}

// Main ------------------------------------------------------------------------

int main()
{
    const struct CMUnitTest tests_creation[] = {
            cmocka_unit_test(test_galaxy_init_2_players)
    };

    int status_creation = cmocka_run_group_tests(
            tests_creation, setup_galaxy_2_players, teardown_galaxy);

    return status_creation;
}