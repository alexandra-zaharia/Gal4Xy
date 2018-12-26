//
// Created by Alexandra Zaharia on 26/12/18.
//

#include <stdlib.h>
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

Player* player_create(char symbol)
{
    Player* player = malloc(sizeof(Player));
    if (!player) {
        MALLOC_ERROR(__func__);
        exit(EXIT_FAILURE);
    }

    player->symbol = symbol;

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
    player->destroy = player_free;

    return player;
}
