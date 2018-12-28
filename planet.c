//
// Created by azaharia on 11/12/18.
//

#include <stdlib.h>
#include "planet.h"
#include "galaxy.h"
#include "error.h"

/*
 * Frees (de-allocates) the planet.
 */
void planet_free(Planet* planet)
{
    if (planet) free(planet);
}


/*
 * Creates and returns a planet at the specified coordinates.
 *
 * Returns NULL in case of failure.
 */
Planet* planet_create(unsigned short int x, unsigned short int y)
{
    Planet* planet = malloc(sizeof(Planet));
    if (!planet) {
        MALLOC_ERROR(__func__, "cannot create planet");
        return NULL;
    }
    planet->owner = NULL;
    planet->x = x;
    planet->y = y;
    planet->res_per_turn = (unsigned short int)(UNIT_COST / 10 * (1 + rand() % 10));
    planet->res_total = 0;

    planet->destroy = planet_free;

    return planet;
}
