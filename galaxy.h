//
// Created by Alexandra Zaharia on 09/12/18.
//

#ifndef GAL4XY_GALAXY_H
#define GAL4XY_GALAXY_H

#include <stdbool.h>
#include "include/vector.h"
#include "planet.h"
#include "fleet.h"

#define SIZE 10            // galaxy size is SIZE x SIZE
#define PLANET_PROB 0.25   // probability for a sector to contain a planet
#define BONUS_MIN 50       // minimum bonus resources for sector exploration
#define BONUS_MAX 250      // maximum bonus resources for sector exploration
#define UNIT_COST 1000     // cost (in resources) for one ship

extern const char O_NONE;  // symbol associated to unowned sectors

typedef struct {
    unsigned short int x;  // x-coordinate of the sector in the galaxy
    unsigned short int y;  // y-coordinate of the sector in the galaxy

    Vector* explored;      // has this sector been explored by the players?

    bool has_planet;       // does this sector contain a planet?
    union {
        unsigned short int res_bonus; // bonus resources for exploration if no planet in sector
        Planet* planet;               // the planet, if applicable
    };

    Fleet* fleet;          // fleet in place in this sector
    Vector* incoming;      // incoming fleets for this sector
} Sector;

typedef struct Galaxy Galaxy;
struct Galaxy {
    Sector*** sectors;                    // pointer to 2D array of pointers to Sector
    Vector* players;

    bool game_over;                       // has the galaxy been conquered?
    unsigned int turn;                    // current turn

    bool (*initialize)(Galaxy*, Vector*); // initializes the galaxy
    void (*display)(Galaxy*);             // display the galaxy
    void (*update)(Galaxy*);              // updates the galaxy when advancing one turn
    void (*destroy)(Galaxy*);             // frees the galaxy
};

Galaxy* galaxy_create();

#endif // galaxy.h
