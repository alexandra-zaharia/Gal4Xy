//
// Created by Alexandra Zaharia on 06/01/19.
//

#include <assert.h>
#include "battle.h"


/*
 * Returns a vector with the players involved in an upcoming conflict in the specified sector.
 */
Vector* players_in_conflict(Sector* sector)
{
    Vector* players = vector_create();

    if (sector->fleet)
        players->add(players, sector->fleet->owner);

    for (unsigned int i = 0; i < sector->incoming->size; i++) {
        Fleet* fleet = sector->incoming->data[i];
        if (!players->contains(players, fleet->owner))
            players->add(players, fleet->owner);
    }

    return players;
}


/*
 * Handles a battle between two players in a given sector.
 */
void battle_between_two_players(Sector* sector, Vector* players, Galaxy* galaxy)
{
    assert(players->size == 2);
    Player* player1 = players->data[0];
    Player* player2 = players->data[1];
}


/*
 * Handles a battle between more than two players in a given sector.
 */
void battle_between_more_than_two_players(Sector* sector, Vector* players, Galaxy* galaxy)
{

}


/*
 * Handles a battle in the given sector.
 */
void battle(Sector* sector, Galaxy* galaxy)
{
    Vector* players = players_in_conflict(sector);
    if (players->size == 2) {
        battle_between_two_players(sector, players, galaxy);
    } else {
        battle_between_more_than_two_players(sector, players, galaxy);
    }
    players->free(players);
}
