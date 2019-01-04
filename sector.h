//
// Created by Alexandra Zaharia on 28/12/18.
//

#ifndef GAL4XY_SECTOR_H
#define GAL4XY_SECTOR_H

#include <stdbool.h>
#include "include/vector.h"
#include "planet.h"
#include "fleet.h"

typedef struct Galaxy Galaxy;

typedef struct Sector Sector;
struct Sector {
    unsigned short int x;             // x-coordinate of the sector in the galaxy
    unsigned short int y;             // y-coordinate of the sector in the galaxy

    Vector* explored;                 // has this sector been explored by the players?

    bool has_planet;                  // does this sector contain a planet?
    union {
        unsigned short int res_bonus; // bonus resources for exploration if no planet in sector
        Planet* planet;               // the planet, if applicable
    };

    Fleet* fleet;                     // fleet in place in this sector
    Vector* incoming;                 // incoming fleets for this sector

    bool (*is_explored)(Sector*, Player*, Galaxy*);   // is the sector explored by the player?
    void (*mark_explored)(Sector*, Player*, Galaxy*); // marks sector as explored for player
    void (*update)(Sector*, Galaxy*); // updates the sector
    void (*destroy)(Sector*);         // frees the sector
};

Sector* sector_create(unsigned short int, unsigned short int);

#endif //GAL4XY_SECTOR_H
