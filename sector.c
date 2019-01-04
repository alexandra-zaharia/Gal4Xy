//
// Created by Alexandra Zaharia on 28/12/18.
//

#include <stdlib.h>
#include <limits.h>
#include "galaxy.h"
#include "player.h"
#include "sector.h"
#include "error.h"
#include "notifications.h"


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
 * Returns the index of a given player among all the players. If the player is not found, returns
 * UINT_MAX.
 */
unsigned int get_player_index(Player* player, Galaxy* galaxy)
{
    for (unsigned int i = 0; i < galaxy->players->size; i++) {
        Player* current_player = galaxy->players->data[i];
        if (player == current_player) {
            return i;
        }
    }

    return UINT_MAX;
}


/*
 * Determines whether the sector has already been explored by the given player.
 */
bool sector_is_explored(Sector* sector, Player* player, Galaxy* galaxy)
{
    unsigned int player_index = get_player_index(player, galaxy);
    return (bool) sector->explored->data[player_index];
}


/*
 * Marks the given sector as explored for the specified player.
 */
void sector_mark_explored(Sector* sector, Player* player, Galaxy* galaxy)
{
    unsigned int player_index = get_player_index(player, galaxy);
    sector->explored->data[player_index] = (void*) true;
}


/*
 * Determines whether there is a conflict in a given sector. A conflict translates to the presence
 * of fleets belonging to at least two players. Fleets may be in place (sector->fleet) or incoming.
 */
bool conflict(Sector* sector)
{
    if (!sector->incoming) return false;
    if (sector->incoming->size > 1) return true;

    // There is precisely one incoming fleet.
    if (!sector->fleet) return false;
    Fleet* incoming = sector->incoming->data[0];
    return sector->fleet->owner != incoming->owner;
}


/*
 * Updates the sector, as it contains incoming fleets.
 */
void sector_update(Sector* sector, Galaxy* galaxy)
{
    if (conflict(sector)) {
        printf("Conflict in sector (%hu, %hu)\n", sector->x, sector->y);
        if (sector->fleet)
            printf("\tFleet (%c): %u power\n", sector->fleet->owner->symbol, sector->fleet->power);
        for (unsigned int i = 0; i < sector->incoming->size; i++) {
            Fleet* incoming = sector->incoming->data[i];
            printf("\tIncoming (%c): %u power\n", incoming->owner->symbol, incoming->power);
        }
    } else {
        Fleet* fleet = (Fleet*) sector->incoming->data[0];
        sector->fleet = fleet;
        fleet->owner->add_fleet(fleet->owner, fleet);
        sector->incoming->remove(sector->incoming, 0);

        sector->mark_explored(sector, fleet->owner, galaxy);

        if (sector->has_planet) {
            sector->planet->owner = fleet->owner;
            fleet->owner->add_planet(fleet->owner, sector->planet);
            if (galaxy->players->data[0] == fleet->owner)
                notify_planet_colonized(sector);
        } else {
            if (sector->res_bonus > 0) {
                Planet* home = fleet->owner->home_planet;
                home->res_total += sector->res_bonus;
                if (galaxy->players->data[0] == fleet->owner)
                    notify_sector_explored(sector, home);
                sector->res_bonus = 0;
            }
        }
    }
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

    sector->is_explored = sector_is_explored;
    sector->mark_explored = sector_mark_explored;
    sector->update = sector_update;
    sector->destroy = sector_free;

    return sector;
}
