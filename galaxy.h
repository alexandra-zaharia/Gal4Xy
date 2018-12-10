//
// Created by Alexandra Zaharia on 09/12/18.
//

#ifndef GAL4XY_GALAXY_H
#define GAL4XY_GALAXY_H

#include <stdbool.h>
#include <stdio.h>
#include "lib/vector.h"

#define MALLOC_ERROR(x) fprintf(stderr, "%s: insufficient memory\n", (x))

#define SIZE 10               // galaxy size is SIZE x SIZE
#define PLANET_PROB 0.25      // probability for a sector to contain a planet
#define BONUS_MIN 50          // minimum bonus resources for sector exploration
#define BONUS_MAX 250         // maximum bonus resources for sector exploration
#define UNIT_COST 1000        // cost (in resources) for one fleet unit

extern const char O_NONE;     // symbol associated to unowned sectors
extern const char O_AI;       // symbol associated to sectors owned by the AI player
extern const char O_HUMAN;    // symbol associated to sectors owned by the human player

typedef struct {
    char owner;
    unsigned short int res_per_turn; // resources generated at every turn
    unsigned short int res_total;    // total resources available on the planet
} Planet;

typedef struct {
    unsigned short int x;           // x-coordinate of the sector in the galaxy
    unsigned short int y;           // y-coordinate of the sector in the galaxy

    bool explored_h;          // has this sector been explored by the human player?
    bool explored_a;          // has this sector been explored by the AI player?

    bool has_planet;          // does this sector contain a planet?
    union {
        unsigned short int res_bonus; // bonus resources for exploration if no planet in sector
        Planet planet;                // the planet, if applicable
    };
} Sector;

typedef struct Galaxy Galaxy;
struct Galaxy {
    Sector** sectors;
    bool game_over;           // has the galaxy been conquered?
    unsigned int turn;        // current turn
    Sector* home_h;           // sector of the human home planet
    Sector* home_a;           // sector of the AI home planet

    void (*display)(Galaxy*); // display the galaxy
};

Galaxy* galaxy_create();
void galaxy_initialize(Galaxy*);
void planet_initialize(Sector*);
void home_planets_initialize(Galaxy*, Vector*);
void galaxy_free(Galaxy*);

#endif // galaxy.h
