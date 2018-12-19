//
// Created by azaharia on 11/12/18.
//

#include <stdlib.h>
#include "planet.h"
#include "galaxy.h"
#include "utils.h"

void planet_free(Planet* planet)
{
    if (planet) free(planet);
}

Planet* planet_create()
{
    Planet* planet = malloc(sizeof(Planet));
    if (!planet) {
        MALLOC_ERROR(__func__);
        exit(EXIT_FAILURE);
    }
    planet->owner = O_NONE;
    planet->res_per_turn = (unsigned short int)(UNIT_COST / 10 * (1 + rand() % 10));
    planet->res_total = 0;

    planet->destroy = planet_free;

    return planet;
}
