#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "include/vector.h"
#include "galaxy.h"
#include "player.h"
#include "ai.h"
#include "io.h"
#include "color.h"

int main()
{
    Galaxy* galaxy = galaxy_create();
    if (!galaxy) exit(EXIT_FAILURE);

    Player* human = player_create('*', BOLDGREEN); // the human player must be the first one
    if (!human) {
        galaxy->destroy(galaxy);
        exit(EXIT_FAILURE);
    }
    Player* ai = player_create('@', BOLDRED);
    if (!ai) {
        human->destroy(human);
        galaxy->destroy(galaxy);
        exit(EXIT_FAILURE);
    }

    Vector* players = vector_create();
    if (!players) {
        fprintf(stderr, "Cannot create players\n");
        exit(EXIT_FAILURE);
    }
    players->add(players, human);
    players->add(players, ai);

    if (!galaxy->initialize(galaxy, players)) {
        galaxy->destroy(galaxy);
        exit(EXIT_FAILURE);
    }

    human->play = prompt;
    ai->play = ai_strategy;

    while (!galaxy->game_over) {
        galaxy->update(galaxy);
        galaxy->display(galaxy);
        ai->play(ai, galaxy);
        human->play(human, galaxy);
    }

    galaxy->destroy(galaxy);

    return EXIT_SUCCESS;
}
