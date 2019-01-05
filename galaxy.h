//
// Created by Alexandra Zaharia on 09/12/18.
//

#ifndef GAL4XY_GALAXY_H
#define GAL4XY_GALAXY_H

#include <stdbool.h>
#include "include/vector.h"
#include "sector.h"

#define SIZE 10            // galaxy size is SIZE x SIZE
#define PLANET_PROB 0.25   // probability for a sector to contain a planet
#define BONUS_MIN 50       // minimum bonus resources for sector exploration
#define BONUS_MAX 250      // maximum bonus resources for sector exploration
#define UNIT_COST 1000     // cost (in resources) for one ship

extern const char O_NONE;  // symbol associated to unowned sectors

typedef struct Galaxy Galaxy;
struct Galaxy {
    Sector*** sectors;                       // pointer to 2D array of pointers to Sector
    Vector* players;

    unsigned int turn;                       // current turn

    bool (*initialize)(Galaxy*, Vector*);    // initializes the galaxy for the given players
    void (*display)(Galaxy*);                // display the galaxy
    void (*update)(Galaxy*);                 // updates the galaxy when advancing one turn
    void (*check_players)(Galaxy*);          // does any player need to be eliminated?
    void (*remove_player)(Galaxy*, Player*); // eliminates a player from the game
    void (*game_over)(Galaxy*);              // displays the game over screen
    void (*destroy)(Galaxy*);                // frees the galaxy
};

Galaxy* galaxy_create();

#endif // galaxy.h
