//
// Created by azaharia on 11/12/18.
//

#ifndef GAL4XY_PLANET_H
#define GAL4XY_PLANET_H

typedef struct Player Player;

typedef struct Planet Planet;
struct Planet {
    unsigned short int x;            // x-coordinate of this planet's sector in the galaxy
    unsigned short int y;            // y-coordinate of the planet's sector in the galaxy
    Player* owner;                   // planet owner
    unsigned int res_per_turn;       // resources generated at every turn
    unsigned int res_total;          // total resources available on the planet

    void (*destroy)(Planet*);        // frees the planet
};

Planet* planet_create(unsigned short int, unsigned short int);

#endif //GAL4XY_PLANET_H
