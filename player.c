//
// Created by Alexandra Zaharia on 26/12/18.
//

#include <stdlib.h>
#include <string.h>
#include "planet.h"
#include "player.h"
#include "error.h"


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
 * Returns the player's fleet at sector (x, y), or NULL in case the player has no fleet at the
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
