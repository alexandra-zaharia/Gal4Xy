#include <stdlib.h>
#include <stdbool.h>
#include "include/vector.h"
#include "galaxy.h"
#include "player.h"
#include "ai.h"
#include "io.h"
#include "color.h"
#include "error.h"

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
    Player* ai2= player_create('~', BOLDYELLOW);
    Player* ai3 = player_create('#', BOLDCYAN);

    Vector* players = vector_create();
    if (!players) {
        MALLOC_ERROR(__func__, "cannot create players");
        human->destroy(human);
        ai->destroy(ai);
        galaxy->destroy(galaxy);
        exit(EXIT_FAILURE);
    }
    players->add(players, human);
    players->add(players, ai);
    players->add(players, ai2);
    players->add(players, ai3);

    if (!galaxy->initialize(galaxy, players)) {
        galaxy->destroy(galaxy);
        exit(EXIT_FAILURE);
    }

    human->play = prompt;
    ai->play = ai_strategy;
    ai2->play = ai_strategy;
    ai3->play = ai_strategy;

    while (true) {
        galaxy->update(galaxy);
        galaxy->display(galaxy);
        for (unsigned int i = 1; i < galaxy->players->size; i++) {
            Player* player = galaxy->players->data[i];
            player->play(player, galaxy);
        }
        /*ai->play(ai, galaxy);
        ai2->play(ai2, galaxy);
        ai3->play(ai3, galaxy);*/
        human->play(human, galaxy);
    }
}
