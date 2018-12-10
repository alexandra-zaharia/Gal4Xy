#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "galaxy.h"
#include "color.h"

int main()
{
    Galaxy* galaxy = galaxy_create();
    galaxy_initialize(galaxy);
    printf(BOLDGREEN "Human (%c) home planet: (%u, %u)\n" RESET,
            galaxy->home_h->planet.owner, galaxy->home_h->x, galaxy->home_h->y);
    printf(BOLDRED "AI (%c) home planet: (%u, %u)\n" RESET,
            galaxy->home_a->planet.owner, galaxy->home_a->x, galaxy->home_a->y);
    galaxy_free(galaxy);

    return EXIT_SUCCESS;
}
