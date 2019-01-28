//
// Created by Alexandra Zaharia on 06/01/19.
//

#include <stdlib.h>
#include <stdio.h> // TODO
#include <assert.h>
#include "battle.h"
#include "player.h"
#include "utils.h"
#include "io.h" // TODO


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
 * Returns the total firepower of a given player in the specified sector.
 */
unsigned int total_firepower(Player* player, Sector* sector)
{
    unsigned int power = 0;

    Fleet* incoming = player->find_incoming(player, sector);
    if (incoming) power += incoming->power;

    return sector->fleet && sector->fleet->owner == player ? power + sector->fleet->power : power;
}


/*
 * Returns the index of the given fleet in the vector of incoming fleets in the specified sector.
 * Returns -1 in case of failure.
 */
int get_index_in_incoming(Fleet* incoming, Sector* sector)
{
    if (!sector->incoming || sector->incoming->size == 0)
        return -1;

    for (unsigned int i = 0; i < sector->incoming->size; i++) {
        Fleet* fleet = sector->incoming->data[i];
        if (incoming == fleet)
            return (int) i;
    }

    return -1;
}


/*
 * Handles battle between two players in the given sector, where player `winner' beats `defeated'.
 */
void player_wins_battle(Player* winner, Player* defeated, Sector* sector, Galaxy* galaxy)
{
    assert(total_firepower(winner, sector) > total_firepower(defeated, sector));

    Fleet* i_winner = winner->find_incoming(winner, sector);
    Fleet* i_defeated = defeated->find_incoming(defeated, sector);

    if (!sector->fleet)
        goto battle_incoming;

    if (sector->fleet->owner != winner) {
        i_winner->power -= sector->fleet->power;
        defeated->remove_fleet(defeated, sector->fleet);
        sector->fleet->destroy(sector->fleet);
        sector->fleet = NULL;
    } else {
        if (sector->fleet->power >= i_defeated->power) {
            sector->fleet->power -= i_defeated->power;
            i_defeated->power = 0;
        } else {
            i_defeated->power -= sector->fleet->power;
            winner->remove_fleet(winner, sector->fleet);
            sector->fleet->destroy(sector->fleet);
            sector->fleet = NULL;

battle_incoming:
            i_winner->power -= i_defeated->power;
            i_defeated->power = 0;
        }
    }

    if (i_defeated) {
        assert(i_defeated->power == 0);
        int index_in_incoming = get_index_in_incoming(i_defeated, sector);
        assert (index_in_incoming >= 0);
        sector->incoming->remove(sector->incoming, index_in_incoming);
        i_defeated->destroy(i_defeated);
    }

    // Mark sector as explored if the defeated player is human, and if the sector contains a planet
    // (for display purposes).
    unsigned int defeated_index = get_player_index(defeated, galaxy);
    if (defeated_index == 0 && sector->has_planet)
        sector->mark_explored(sector, defeated, galaxy);
}


/*
 * Handles a battle in the given sector ending up in a tie.
 */
void battle_at_tie(Sector* sector) {
    if (sector->fleet) {
        Player *fleet_owner = sector->fleet->owner;
        fleet_owner->remove_fleet(fleet_owner, sector->fleet);
        sector->fleet->destroy(sector->fleet);
        sector->fleet = NULL;
    }

    unsigned int number_of_incoming_fleets = sector->incoming->size;
    for (unsigned int i = 0; i < number_of_incoming_fleets; i++) {
        Fleet* incoming = sector->incoming->data[i];
        incoming->destroy(incoming);
        sector->incoming->remove(sector->incoming, (int) (number_of_incoming_fleets - i - 1));
    }
}


/*
 * Handles a battle between two players in the given sector. Returns the winner or NULL if the
 * players battle to tie.
 */
Player* battle_between_two_players(Vector* players, Sector* sector, Galaxy* galaxy)
{
    assert(players->size == 2);

    Player* player1 = players->data[0];
    Player* player2 = players->data[1];
    Player* winner = NULL;

    unsigned int power1 = total_firepower(player1, sector);
    unsigned int power2 = total_firepower(player2, sector);

    if (power1 < power2) {
        player_wins_battle(player2, player1, sector, galaxy);
        winner = player2;
    } else if (power1 > power2) {
        player_wins_battle(player1, player2, sector, galaxy);
        winner = player1;
    } else { // tie
        battle_at_tie(sector);
        sector->mark_at_tie(sector, player1, galaxy);
        sector->mark_at_tie(sector, player2, galaxy);
    }

    return winner;
}


/*
 * Handles a battle between more than two players in the given sector. Returns the winner or NULL if
 * the players battle to tie.
 */
Player* battle_between_more_than_two_players(Vector* players, Sector* sector, Galaxy* galaxy)
{
    Player* winner = NULL;
    shuffle(players->data, players->size); // random order for attack

    return winner;
}


/*
 * Handles a battle in the given sector. Returns the winner or NULL if the players battle to tie.
 */
Player* battle(Sector* sector, Galaxy* galaxy)
{
    Vector* players = players_in_conflict(sector);
    Player* winner = NULL;

    if (players->size == 2) {
        winner = battle_between_two_players(players, sector, galaxy);
    } else {
        winner = battle_between_more_than_two_players(players, sector, galaxy);
    }

    players->free(players);
    return winner;
}
