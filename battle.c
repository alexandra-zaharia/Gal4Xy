//
// Created by Alexandra Zaharia on 06/01/19.
//

#include <stdlib.h>
#include <stdio.h> // TODO
#include <assert.h>
#include "battle.h"
#include "player.h"
#include "error.h"
#include "utils.h"
#include "circular_linked_list.h"
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
 * Handles battle between two players in the given sector, where player `winner' beats `defeated'.
 */
void player_wins_battle(Player* winner, Player* defeated, Sector* sector, Galaxy* galaxy)
{
    assert(total_firepower(winner, sector) > total_firepower(defeated, sector));

    Fleet* i_winner = winner->find_incoming(winner, sector);
    Fleet* i_defeated = defeated->find_incoming(defeated, sector);

    if (!sector->fleet)
        goto battle_incoming;

    if (sector->fleet->owner == defeated) {
        i_winner->power -= sector->fleet->power;
        defeated->remove_fleet(defeated, sector->fleet);
        sector->fleet->destroy(sector->fleet);
        sector->fleet = NULL;
    } else if (sector->fleet->owner == winner) {
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
        int index_in_incoming = sector->incoming->index(sector->incoming, i_defeated);
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
        Player* fleet_owner = sector->fleet->owner;
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


typedef struct PlayerPower {
    Player* player;
    int power;
} PlayerPower;


/*
 * Returns a shuffled circular linked list of players. The input Vector* is also shuffled. Returns
 * NULL in case of failure.
 */
CircularLinkedList* shuffle_players(Vector* players, Sector* sector)
{
    CircularLinkedList* shuffled_player_power = circular_linked_list_create();
    shuffle(players->data, players->size); // random order for attack

    for (unsigned int i = 0; i < players->size; i++) {
        Player* player = players->data[i];
        unsigned int power = total_firepower(player, sector);
        PlayerPower* player_power = malloc(sizeof(PlayerPower));
        if (!player_power) {
            MALLOC_ERROR(__func__, "cannot create player power variable");
            shuffled_player_power->free(shuffled_player_power);
            return NULL;
        }
        player_power->player = player;
        player_power->power = (int) power;
        shuffled_player_power->insert_end(shuffled_player_power, player_power);
    }

    return shuffled_player_power;
}


/*
 * Removes a player's incoming and in place fleets in the given sector, if they exist.
 */
void _remove_fleets(PlayerPower* player_power, Sector* sector)
{
    Fleet* incoming = player_power->player->find_incoming(player_power->player, sector);
    if (incoming) {
        int index = sector->incoming->index(sector->incoming, incoming);
        sector->incoming->remove(sector->incoming, index);
        incoming->destroy(incoming);
    }

    Fleet* in_place = player_power->player->find_fleet(player_power->player, sector);
    if (in_place) {
        player_power->player->remove_fleet(player_power->player, in_place);
        sector->fleet->destroy(sector->fleet);
        sector->fleet = NULL;
    }
}


/*
 * Removes a PlayerPower variable from the circular linked list, if it corresponds to a player
 * having lost a 1-vs-1 battle during a confrontation between more than two players.
 */
void _remove_defeated(CircularLinkedList* shuffled_player_power, PlayerPower* player_power)
{
    int index = shuffled_player_power->index(shuffled_player_power, player_power);
    free(player_power);
    shuffled_player_power->remove_at(shuffled_player_power, index);
}


/*
 * Handles a battle between more than two players in the given sector. Returns the winner or NULL in
 * case the players battle to tie.
 */
Player* battle_between_more_than_two_players(Vector* players, Sector* sector, Galaxy* galaxy)
{
    CircularLinkedList* shuffled_player_power = shuffle_players(players, sector);
    if (!shuffled_player_power) {
        players->free(players);
        galaxy->destroy(galaxy);
        exit(EXIT_FAILURE);
    }

    DNode* node = shuffled_player_power->head;
    while (node && shuffled_player_power->size > 1) {
        PlayerPower* p_curr = node->data;
        PlayerPower* p_next = node->next->data;

        if (p_curr->power < p_next->power) {
            player_wins_battle(p_next->player, p_curr->player, sector, galaxy);
        } else if (p_curr->power > p_next->power) {
            player_wins_battle(p_curr->player, p_next->player, sector, galaxy);
        } else {
            _remove_fleets(p_curr, sector);
            _remove_fleets(p_next, sector);
        }

        int power_curr = p_curr->power;
        p_curr->power -= p_next->power;
        p_next->power -= power_curr;

        if (p_curr->power <= 0) {
            node = node->prev;
            _remove_defeated(shuffled_player_power, p_curr);
        }
        if (p_next->power <= 0) {
            if (node->data == p_next)
                node = shuffled_player_power->size == 1 ? NULL : node->prev;
            _remove_defeated(shuffled_player_power, p_next);
        }

        if (node) node = node->next;
    }

    Player* winner;
    if (shuffled_player_power->size == 0) {
        battle_at_tie(sector);
        for (unsigned int i = 0; i < players->size; i++)
            sector->mark_at_tie(sector, players->data[i], galaxy);
        winner = NULL;
    } else {
        winner = ((PlayerPower *) shuffled_player_power->head->data)->player;
        free(shuffled_player_power->head->data);
    }

    shuffled_player_power->free(shuffled_player_power);
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
