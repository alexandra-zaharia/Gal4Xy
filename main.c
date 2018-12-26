#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "include/vector.h"
#include "galaxy.h"
#include "player.h"
#include "ai.h"
#include "io.h"
#include "color.h"

void initialize_sectors(Galaxy* galaxy, Vector* players)
{
    for (unsigned int i = 0; i < SIZE; i++)
        for (unsigned int j = 0; j < SIZE; j++) {
            Sector* sector = &galaxy->sectors[i][j];
            for (unsigned int k = 0; k < players->size; k++) {
//                sector->explored->add(sector->explored, false);
            }
        }
}

int main()
{
    Vector* players = vector_create();
    if (!players) {
        fprintf(stderr, "Cannot create players\n");
        exit(EXIT_FAILURE);
    }

    Player* human = player_create('*');
    Player* ai = player_create('@');
    players->add(players, human);
    players->add(players, ai);

    Galaxy* galaxy = galaxy_create();
    galaxy->players = players;
    galaxy->initialize(galaxy);

    human->play = prompt;
    ai->play = ai_strategy;
    initialize_sectors(galaxy, players);

    Planet* human_home = human->home_planet(human);
    Planet* ai_home = ai->home_planet(ai);
    printf(BOLDGREEN "Human (%c) home planet: (%u, %u)\n" RESET,
            human_home->owner, human_home->x, human_home->y);
    printf(BOLDRED "AI (%c) home planet: (%u, %u)\n" RESET,
            ai_home->owner, ai_home->x, ai_home->y);

    while (!galaxy->game_over) {
        printf("Game over? %d\n", galaxy->game_over);
        galaxy->display(galaxy);
        human->play(human, galaxy);
        ai->play(ai, galaxy);
        ++galaxy->turn;
    }

    human->destroy(human);
    ai->destroy(ai);
    players->free(players);
    galaxy->destroy(galaxy);

    return EXIT_SUCCESS;
}
