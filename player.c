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
 * Returns the player's incoming fleet in the specified sector, or NULL in case the player has no
 * incoming fleet there.
 */
Fleet* player_find_incoming_fleet(Player* player, Sector* sector)
{
    for (unsigned int i = 0; i < sector->incoming->size; i++) {
        Fleet* fleet = (Fleet*) sector->incoming->data[i];
        if (fleet->owner == player)
            return fleet;
    }
    return NULL;
}


/*
 * Returns the player's fleet the specified sector, or NULL in case the player has no fleet there.
 */
Fleet* player_find_fleet(Player* player, Sector* sector)
{
    for (DNode* node = player->fleets->head; node; node = node->next) {
        Fleet* fleet = (Fleet*) node->data;
        if (fleet->x == sector->x && fleet->y == sector->y)
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
void player_move_fleet(
        Player* player, Galaxy* galaxy, int _sx, int _sy, int _tx, int _ty, int _power)
{
    if (is_move_valid(player, galaxy, _sx, _sy, _tx, _ty, _power)) {
        unsigned short int sx = (unsigned short int) _sx;
        unsigned short int sy = (unsigned short int) _sy;
        unsigned short int tx = (unsigned short int) _tx;
        unsigned short int ty = (unsigned short int) _ty;
        unsigned int power = (unsigned int) _power;

        // Add ships to or create incoming fleet in sector (tx, ty)
        Fleet* f_dst = player->find_incoming(player, galaxy->sectors[tx][ty]);

        if (f_dst) {
            f_dst->power += power;
        } else {
            Fleet* fleet = fleet_create(tx, ty, player, power);
            galaxy->sectors[tx][ty]->incoming->add(galaxy->sectors[tx][ty]->incoming, fleet);
        }

        // Remove ships from or delete fleet at sector (sx, sy)
        Fleet* f_src = player->find_fleet(player, galaxy->sectors[sx][sy]);
        assert(f_src);
        f_src->power -= power;
        if (f_src->power == 0) {
            int index = player->fleets->index(player->fleets, f_src);
            assert(index >= 0);
            player->fleets->remove_at(player->fleets, index);
            galaxy->sectors[sx][sy]->fleet->destroy(galaxy->sectors[sx][sy]->fleet);
            galaxy->sectors[sx][sy]->fleet = NULL;
        }

        // Log moving fleets for human player
        if (galaxy->players->data[0] == player) {
            log_move_fleet(sx, sy, tx, ty, power);
            galaxy->display(galaxy);
        }
    }
}


/*
 * Returns the index in the player's list of fleets where a new fleet is to be inserted, such that
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
        if (to_insert->x == fleet->x && to_insert->y < fleet->y) return index;
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
        player->fleets->insert_at(player->fleets, fleet, index);
    }
}


/*
 * Removes a fleet from the player's list of fleets.
 */
void player_remove_fleet(Player* player, Fleet* fleet)
{
    int index = player->fleets->index(player->fleets, fleet);
    assert(index >= 0);
    player->fleets->remove_at(player->fleets, index);
}


/*
 * Returns the index in the player's list of planets where a new planet is to be inserted, such
 * that the player's planets are sorted by ascending sector coordinates. Returns -1 if the new
 * planet needs to be inserted at the end of the player's list of planets.
 */
int player_insert_planet_at_index(Player* player, Planet* to_insert)
{
    int index = -1;

    for (DNode* node = player->planets->head; node; node = node->next) {
        ++index;
        Planet* planet = node->data;
        if (to_insert->x < planet->x) return index;
        if (to_insert->x == planet->x && to_insert->y < planet->y) return index;
    }

    return -1;
}


/*
 * Adds a new planet to the player's list of planets, such that the player's planets are sorted by
 * ascending sector coordinates.
 */
void player_add_planet(Player* player, Planet* planet)
{
    int index = player_insert_planet_at_index(player, planet);
    if (index < 0) {
        player->planets->insert_end(player->planets, planet);
    } else {
        player->planets->insert_at(player->planets, planet, index);
    }
}


/*
 * Removes a planet from the player's list of planets. Updates the home planet if necessary. If this
 * is not possible, eliminates the player from the game.
 */
void player_remove_planet(Player* player, Planet* planet, Galaxy* galaxy)
{
    int index = player->planets->index(player->planets, planet);
    assert(index >= 0);
    player->planets->remove_at(player->planets, index);
    if (get_player_index(player, galaxy) == 0) // notify human player that a planet has been lost
        notify_planet_lost(planet);

    if (player->home_planet == planet) {
        player->reassign_home_planet(player);
    }
}


/*
 * Assigns a new planet as the player's home planet. Does nothing if the player has no more planets.
 */
void player_reassign_home_planet(Player* player)
{
    if (player->planets->size == 0)
        return;

    unsigned short int rnd = random_number(0, (unsigned short int) (player->planets->size - 1));
    unsigned short int i = 0;
    DNode* node = player->planets->head;
    while (i++ < rnd) {
        node = node->next;
    }
    Planet* new_home = node->data;
    player->home_planet = new_home;
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
                player->add_fleet(player, sector->fleet);
            } else {
                sector->fleet->power += power;
            }
            planet->res_total -= (unsigned short int) (power * UNIT_COST);

            // Notify the human player that ships were built
            if (get_player_index(player, galaxy) == 0)
                notify_ships_built(sector, power);
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
    player->add_fleet = player_add_fleet;
    player->remove_fleet = player_remove_fleet;
    player->add_planet = player_add_planet;
    player->remove_planet = player_remove_planet;
    player->reassign_home_planet = player_reassign_home_planet;
    player->find_fleet = player_find_fleet;
    player->find_incoming = player_find_incoming_fleet;
    player->move_fleet = player_move_fleet;
    player->update_resources = player_update_resources;
    player->build_ships = player_build_ships;
    player->destroy = player_free;

    player->symbol = symbol;
    player->color = color;

    player->is_retired = false;
    player->retirement_turn = 0;

    player->home_planet = NULL;
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
