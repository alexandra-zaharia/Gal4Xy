//
// Created by Alexandra Zaharia on 26/12/18.
//

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "planet.h"
#include "player.h"
#include "error.h"
#include "notifications.h"
#include "utils.h"
#include "io.h"


/*
 * Returns the player's home planet, or NULL if the player has no more planets left.
 */
Planet* get_player_home_planet(Player* player)
{
    if (!player->planets || !player->planets->head) return NULL;
    return (Planet*) player->planets->head->data;
}


/*
 * Frees (de-allocates) the player.
 */
void player_free(Player* player)
{
    if (!player) return;

    if (player->planets) player->planets->free(player->planets);
    if (player->fleets) player->fleets->free(player->fleets);

    free(player);
}

/*
 * Determines whether the specified color is a valid color for a player. YELLOW is reserved for '?'
 * in cheat mode, and (BOLD)WHITE and (BOLD)BLACK are reserved for console display.
 */
bool is_color_valid(char* color)
{
    return !strcmp(color, RED) || !strcmp(color, GREEN)
        || !strcmp(color, BLUE) || !strcmp(color, MAGENTA) || !strcmp(color, CYAN)
        || !strcmp(color, BOLDRED) || !strcmp(color, BOLDGREEN) || !strcmp(color, BOLDYELLOW)
        || !strcmp(color, BOLDBLUE) || !strcmp(color, BOLDMAGENTA) || !strcmp(color, BOLDCYAN);
}


/*
 * Updates resources for every planet owned by the player.
 */
void player_update_resources(Player* player)
{
    for (DNode* node = player->planets->head; node; node = node->next) {
        Planet* planet = (Planet*) node->data;
        planet->res_total += planet->res_per_turn;
    }
}

/*
 * Returns the player's incoming fleet in sector (x, y), or NULL in case the player has no incoming
 * fleet at the specified location.
 */
Fleet* player_find_incoming_fleet(
        Player* player, Galaxy* galaxy, unsigned short int x, unsigned short int y)
{
    for (unsigned int i = 0; i < galaxy->sectors[x][y]->incoming->size; i++) {
        Fleet* fleet = (Fleet*) galaxy->sectors[x][y]->incoming->data[i];
        if (fleet->owner == player)
            return fleet;
    }
    return NULL;
}


/*
 * Returns the player's fleet in sector (x, y), or NULL in case the player has no fleet at the
 * specified location.
 */
Fleet* player_find_fleet(Player* player, unsigned short int x, unsigned short int y)
{
    for (DNode* node = player->fleets->head; node; node = node->next) {
        Fleet* fleet = (Fleet*) node->data;
        if (fleet->x == x && fleet->y == y)
            return fleet;
    }
    return NULL;
}


/*
 * Determines whether a player may move a fleet of specified power from sector (sx, sy) to sector
 * (tx, ty).
 */
bool is_move_valid(Player* player, Galaxy* galaxy,
        int const sx, int const sy,
        int const tx, int const ty,
        int const power)
{
    if (sx < 0 || sy < 0 || tx < 0 || ty < 0) {
        SECTOR_COORD_NEGATIVE_ERROR;
        return false;
    }

    if (sx >= SIZE || sy >= SIZE || tx >= SIZE || ty >= SIZE) {
        SECTOR_COORD_TOO_BIG_ERROR;
        return false;
    }

    if (power < 1) {
        FLEET_POWER_ERROR;
        return false;
    }

    if (sx == tx && sy == ty) {
        SECTOR_FROM_TO_ERROR;
        return false;
    }

    if (!galaxy->sectors[sx][sy]->fleet || galaxy->sectors[sx][sy]->fleet->owner != player) {
        NOT_FLEET_OWNER_ERROR((unsigned short int) sx, (unsigned short int) sy);
        return false;
    }

    if (galaxy->sectors[sx][sy]->fleet->power < (unsigned int) power) {
        FLEET_TOO_SMALL_ERROR((unsigned short int) sx, (unsigned short int) sy);
        return false;
    }

    return true;
}


/*
 * Attempts to move a player's fleet of specified power, from sector (sx, sy) to sector (tx, ty).
 */
void player_move_fleet(Player* player, Galaxy* galaxy, int sx, int sy, int tx, int ty, int power)
{
    if (is_move_valid(player, galaxy, sx, sy, tx, ty, power)) {
        // Add ships to or create incoming fleet in sector (tx, ty)
        Fleet* f_dst = player->find_incoming(
                player, galaxy, (unsigned short int) tx, (unsigned short int) ty);

        if (f_dst) {
            f_dst->power += (unsigned int) power;
        } else {
            galaxy->sectors[sx][sy]->incoming->add(galaxy->sectors[sx][sy]->incoming, f_dst);
        }

        // Remove ships from or delete fleet at sector (sx, sy)
        Fleet* f_src = player->find_fleet(player, (unsigned short int) sx, (unsigned short int) sy);
        assert(f_src);
        f_src->power -= (unsigned int) power;
        if (f_src->power == 0) {
            int index = get_index_in_list(player->fleets, f_src);
            assert(index >= 0);
            player->fleets->remove_at(player->fleets, (unsigned int) index);
            galaxy->sectors[sx][sy]->fleet->destroy(galaxy->sectors[sx][sy]->fleet);
            galaxy->sectors[sx][sy]->fleet = NULL;
        }

        // Log moving fleets for human player
        if (galaxy->players->data[0] == player)
            log_move_fleet((unsigned short int) sx, (unsigned short int) sy,
                           (unsigned short int) tx, (unsigned short int) ty,
                           (unsigned int) power);
    }
}


/*
 * Returns the index in the player's list of fleets at which to insert a new given fleet, such that
 * the player's fleets are sorted by ascending sector coordinates. Returns -1 if the new fleet
 * needs to be inserted at the end of the player's list of fleets.
 */
int player_insert_fleet_at_index(Player* player, Fleet* to_insert)
{
    int index = -1;

    for (DNode* node = player->fleets->head; node; node = node->next) {
        ++index;
        Fleet* fleet = node->data;
        if (to_insert->x < fleet->x) return index;
        if (to_insert->x == fleet->x) {
            if (to_insert->y < fleet->y) return index;
            return index + 1;
        }
    }

    return -1;
}


/*
 * Adds a new fleet to the player's list of fleets, such that the player's fleets are sorted by
 * ascending sector coordinates.
 */
void player_add_fleet(Player* player, Fleet* fleet)
{
    int index = player_insert_fleet_at_index(player, fleet);
    if (index < 0) {
        player->fleets->insert_end(player->fleets, fleet);
    } else {
        player->fleets->insert_at(player->fleets, fleet, (unsigned int) index);
    }
}


/*
 * Builds ships on every planet owned by the player having enough resources.
 */
void player_build_ships(Player* player, Galaxy* galaxy)
{
    for (DNode* node = player->planets->head; node; node = node->next) {
        Planet *planet = (Planet *) node->data;
        if (planet->res_total >= UNIT_COST) {
            unsigned int power = (unsigned int) (planet->res_total)/UNIT_COST;
            Sector* sector = galaxy->sectors[planet->x][planet->y];
            if (!sector->fleet) {
                sector->fleet = fleet_create(sector->x, sector->y, player, power);
                player_add_fleet(player, sector->fleet);
            } else {
                sector->fleet->power += power;
            }
            planet->res_total -= (unsigned short int) (power * UNIT_COST);

            // Log built ships for human player
            if (galaxy->players->data[0] == player)
                notification_ships_built(sector->x, sector->y, power);
        }
    }
}


/*
 * Creates and returns a player with an associated symbol and color as specified.
 *
 * Returns NULL in case of failure.
 */
Player* player_create(char symbol, char* color)
{
    if (!is_color_valid(color)) {
        INVALID_COLOR_ERROR(__func__, color);
        return NULL;
    }

    Player* player = malloc(sizeof(Player));
    if (!player) {
        MALLOC_ERROR(__func__, "cannot create player");
        return NULL;
    }

    player->play = NULL;
    player->home_planet = get_player_home_planet;
    player->find_fleet = player_find_fleet;
    player->find_incoming = player_find_incoming_fleet;
    player->move_fleet = player_move_fleet;
    player->update_resources = player_update_resources;
    player->build_ships = player_build_ships;
    player->destroy = player_free;

    player->symbol = symbol;
    player->color = color;

    player->fleets = NULL;
    player->planets = linked_list_create();
    if (!player->planets) {
        MALLOC_ERROR(__func__, "cannot create planet list");
        player->destroy(player);
        return NULL;
    }

    player->fleets = linked_list_create();
    if (!player->fleets) {
        MALLOC_ERROR(__func__, "cannot create fleet list");
        player->destroy(player);
        return NULL;
    }

    return player;
}
