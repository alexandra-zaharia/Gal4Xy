//
// Created by Alexandra Zaharia on 27/12/18.
//

#include <stdlib.h>
#include "fleet.h"
#include "error.h"


/*
 * Frees the fleet.
 */
void fleet_free(Fleet* fleet)
{
    if (fleet) free(fleet);
}


/*
 * Creates and returns a fleet at the specified coordinates, belonging to the specified player, and
 * having the specified firepower.
 *
 * Returns NULL in case of failure.
 */
Fleet* fleet_create(unsigned short int x, unsigned short int y, Player* owner, unsigned int power)
{
    Fleet* fleet = malloc(sizeof(Fleet));
    if (!fleet) {
        MALLOC_ERROR(__func__, "cannot create fleet");
        return NULL;
    }

    fleet->x = x;
    fleet->y = y;
    fleet->owner = owner;
    fleet->power = power;

    fleet->destroy = fleet_free;

    return fleet;
}


/*
 * Creates and returns a FleetDeployment structure containing the specified fleet and sector.
 * Returns NULL in case of failure.
 */
FleetDeployment* fleet_deployment_create(Fleet* fleet, Sector* target)
{
    FleetDeployment* fd = malloc(sizeof(FleetDeployment));
    if (!fd) {
        MALLOC_ERROR(__func__, "cannot create fleet deployment variable");
        return NULL;
    }

    fd->fleet = fleet;
    fd->target = target;

    return fd;
}
