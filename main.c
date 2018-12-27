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
    Player* human = player_create('*', BOLDGREEN);
    Player* ai = player_create('@', BOLDRED);

    Vector* players = vector_create();
    if (!players) {
        fprintf(stderr, "Cannot create players\n");
        exit(EXIT_FAILURE);
    }
    players->add(players, human);
    players->add(players, ai);

    Galaxy* galaxy = galaxy_create();
    galaxy->initialize(galaxy, players);

    human->play = prompt;
    ai->play = ai_strategy;

    while (!galaxy->game_over) {
        galaxy->display(galaxy);
        ai->play(ai, galaxy);
        human->play(human, galaxy);
        ++galaxy->turn;
    }

    galaxy->destroy(galaxy);

    return EXIT_SUCCESS;
}
