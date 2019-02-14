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
 * Randomly shuffles the `data' array of size `size' in place. Also known as the Knuth shuffle.
 */
void shuffle(void** data, unsigned int size)
{
    for (unsigned int i = size-1; i > 0; i--) {
        unsigned int j = random_number(0, (unsigned short int) i);
        void* tmp = data[i];
        data[i] = data[j];
        data[j] = tmp;
    }
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
