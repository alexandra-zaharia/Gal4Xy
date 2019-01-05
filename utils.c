//
// Created by azaharia on 11/12/18.
//

#include "player.h"
#include "galaxy.h"
#include <stdlib.h>
#include <limits.h>
#include "utils.h"

/*
 * Returns a random (unsigned short) integer between min and max, inclusive.
 */
unsigned short int random_number(unsigned short int min, unsigned short int max)
{
    return (unsigned short int)(min + rand() % (max - min + 1));
}

/*
 * Returns the index in the given list equal to item, or -1 in case the item is not in the list.
 */
int get_index_in_list(LinkedList* list, void* item)
{
    int index = -1;
    for (DNode* node = list->head; node; node = node->next) {
        ++index;
        if (node->data == item)
            return index;
    }
    return -1;
}

/*
 * Returns true if the sector has an incoming fleet belonging to the specified player.
 */
bool has_incoming_fleet(Sector* sector, Player* player)
{
    if (!sector->incoming) return false;
    if (sector->incoming->size == 0) return false;

    for (unsigned int i = 0; i < sector->incoming->size; i++) {
        Fleet* fleet = sector->incoming->data[i];
        if (fleet->owner == player)
            return true;
    }

    return false;
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
