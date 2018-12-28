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
    player->update_resources = player_update_resources;
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
