//
// Created by azaharia on 11/12/18.
//

#ifndef GAL4XY_PLANET_H
#define GAL4XY_PLANET_H

typedef struct {
    char owner;
    unsigned short int res_per_turn; // resources generated at every turn
    unsigned short int res_total;    // total resources available on the planet
} Planet;

#endif //GAL4XY_PLANET_H
