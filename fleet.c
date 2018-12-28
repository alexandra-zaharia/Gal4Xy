//
// Created by Alexandra Zaharia on 27/12/18.
//

#include <stdlib.h>
#include "fleet.h"
#include "error.h"


/*
 * Moves a specified firepower from a fleet to a specified sector.
 *
 * The fleet is not actually moved during the current turn, but marked as incoming fleet for the
 * next turn.
 *
 * If the firepower of the initial fleet is zero after moving a quantity of 'power' to the specified
 * sector, the initial fleet is de-allocated.
 */
void fleet_move(Fleet* fleet, unsigned short int x, unsigned short int y, unsigned int power)
{
    printf("%s: not implemented\n", __func__);
}


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

    fleet->move = fleet_move;
    fleet->destroy = fleet_free;

    return fleet;
}
