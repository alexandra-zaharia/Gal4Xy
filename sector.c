//
// Created by Alexandra Zaharia on 28/12/18.
//

#include <malloc.h>
#include "galaxy.h"
#include "sector.h"
#include "error.h"


/*
 * Frees (de-allocates) the sector.
 */
void sector_free(Sector* sector)
{
    if (!sector) return;

    if (sector->explored)
        sector->explored->free(sector->explored);

    if (sector->has_planet && sector->planet)
        sector->planet->destroy(sector->planet);

    if (sector->fleet)
        sector->fleet->destroy(sector->fleet);

    if (sector->incoming) {
        for (unsigned int i = 0; i < sector->incoming->size; i++) {
            Fleet* fleet = (Fleet*) sector->incoming->data[i];
            fleet->destroy(fleet);
        }
        sector->incoming->free(sector->incoming);
    }

    free(sector);
}


/*
 * Updates the sector, as it contains incoming fleets.
 */
void sector_update(Sector* sector, Galaxy* galaxy)
{
    printf("sector update: (%hu, %hu)\n", sector->x, sector->y);
}


/*
 * Creates and returns a sector at the specified coordinates.
 *
 * Returns NULL in case of failure.
 */
Sector* sector_create(unsigned short int x, unsigned short int y)
{
    Sector* sector = malloc(sizeof(Sector));
    if (!sector) {
        MALLOC_ERROR(__func__, "cannot create sector");
        return NULL;
    }

    sector->x = x;
    sector->y = y;
    sector->explored = NULL;
    sector->has_planet = false;
    sector->fleet = NULL;
    sector->incoming = NULL;

    sector->update = sector_update;
    sector->destroy = sector_free;

    return sector;
}