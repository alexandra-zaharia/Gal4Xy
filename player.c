//
// Created by Alexandra Zaharia on 26/12/18.
//

#include <stdlib.h>
#include <string.h>
#include "planet.h"
#include "utils.h"
#include "player.h"


Planet* get_player_home_planet(Player* player)
{
    if (!player->planets || !player->planets->head) return NULL;
    return (Planet*) player->planets->head->data;
}


void player_free(Player* player)
{
    if (!player) return;

    player->planets->free(player->planets);
    player->fleets->free(player->fleets);
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


Player* player_create(char symbol, char* color)
{
    if (!is_color_valid(color)) {
        fprintf(stderr, "%s: invalid color (%s)\n", __func__, color);
        exit(EXIT_FAILURE);
    }

    Player* player = malloc(sizeof(Player));
    if (!player) {
        MALLOC_ERROR(__func__);
        exit(EXIT_FAILURE);
    }

    player->symbol = symbol;
    player->color = color;

    player->planets = linked_list_create();
    if (!player->planets) {
        MALLOC_ERROR(__func__);
        free(player);
        exit(EXIT_FAILURE);
    }

    player->fleets = linked_list_create();
    if (!player->fleets) {
        MALLOC_ERROR(__func__);
        player->planets->free(player->planets);
        free(player);
        exit(EXIT_FAILURE);
    }

    player->play = NULL;
    player->home_planet = get_player_home_planet;
    player->update_resources = player_update_resources;
    player->destroy = player_free;

    return player;
}
