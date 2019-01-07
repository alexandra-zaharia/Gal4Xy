//
// Created by Alexandra Zaharia on 06/01/19.
//

#include "battle.h"


/*
 * Returns the number of players involved in an upcoming conflict in the specified sector.
 */
unsigned int number_of_players_in_conflict(Sector* sector)
{
    Vector* players = vector_create();

    if (sector->fleet)
        players->add(players, sector->fleet->owner);

    for (unsigned int i = 0; i < sector->incoming->size; i++) {
        Fleet* fleet = sector->incoming->data[i];
        if (!players->contains(players, fleet->owner))
            players->add(players, fleet->owner);
    }

    unsigned int number_of_players = players->size;
    players->free(players);
    return number_of_players;
}

/*
 * Handles a battle in the given sector.
 */
void battle(Sector* sector, Galaxy* galaxy)
{

}
