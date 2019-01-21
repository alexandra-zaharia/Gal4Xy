//
// Created by Alexandra Zaharia on 28/12/18.
//

#include <stdlib.h>
#include <assert.h>
#include "galaxy.h"
#include "player.h"
#include "sector.h"
#include "battle.h"
#include "utils.h"
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
        notify_battle_header(sector, galaxy);
        Player* winner = battle(sector, galaxy);
        notify_battle_summary(winner);
    }

    if (sector->incoming->size == 0) return;
    assert(sector->incoming->size == 1);

    Fleet* incoming = sector->incoming->data[0];
    Player* owner = incoming->owner;

    Fleet* in_place = owner->find_fleet(owner, sector);
    if (in_place) {
        in_place->power += incoming->power;
        incoming->destroy(incoming);
    } else {
        owner->add_fleet(owner, incoming);
        sector->fleet = incoming;
    }

    sector->incoming->remove(sector->incoming, 0);
    sector->mark_explored(sector, owner, galaxy);

    if (sector->has_planet) {
        Player* old_owner = sector->planet->owner;

        if (!old_owner || old_owner != owner) {
            sector->planet->owner = owner;
            owner->add_planet(owner, sector->planet);

            if (get_player_index(owner, galaxy) == 0)
                notify_planet_colonized(sector);

            // The planet had been owned by another player but left undefended
            if (old_owner && old_owner != owner) {
                old_owner->remove_planet(old_owner, sector->planet, galaxy);
                sector->planet->res_total = 0;
            }
        }
    } else if (sector->res_bonus > 0) {
        Planet *home = owner->home_planet;
        home->res_total += sector->res_bonus;

        if (get_player_index(owner, galaxy) == 0)
            notify_sector_explored(sector, home);

        sector->res_bonus = 0;
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
