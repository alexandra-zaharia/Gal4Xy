//
// Created by Alexandra Zaharia on 06/01/19.
//

#include <stdlib.h>
#include <assert.h>
#include "battle.h"
#include "player.h"
#include "error.h"
#include "utils.h"
#include "notifications.h"
#include "circular_linked_list.h"


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
        if (i_defeated->power > 0) {
            assert(i_winner && i_winner->power > 0);
            i_winner->power -= i_defeated->power;
            i_defeated->power = 0;
        }
        assert(i_defeated->power == 0);
        int index_in_incoming = sector->incoming->index(sector->incoming, i_defeated);
        assert (index_in_incoming >= 0);
        sector->incoming->remove(sector->incoming, index_in_incoming);
        i_defeated->destroy(i_defeated);
    }

    // Mark sector as explored if the defeated player is human, and if the sector contains a planet
    // (for display purposes).
    int defeated_index = galaxy->players->index(galaxy->players, defeated);
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


typedef struct BattlingPlayer {
    Player* player;
    int power;
} BattlingPlayer;

/*
 * Returns a circular linked list, containing the players in the input Vector*, in the same order.
 * Returns NULL in case of failure.
 */
CircularLinkedList* get_battling_players(Vector* players, Sector* sector)
{
    CircularLinkedList* battling_players = circular_linked_list_create();

    for (unsigned int i = 0; i < players->size; i++) {
        BattlingPlayer* battling_player = malloc(sizeof(BattlingPlayer));
        if (!battling_player) {
            MALLOC_ERROR(__func__, "cannot create player power variable");
            battling_players->free(battling_players);
            return NULL;
        }
        battling_player->player = players->data[i];
        battling_player->power = (int) total_firepower(players->data[i], sector);
        battling_players->insert_end(battling_players, battling_player);
    }

    return battling_players;
}


/*
 * Removes a player's incoming and in place fleets in the given sector, if they exist.
 */
void _remove_fleets(BattlingPlayer* battling_player, Sector* sector)
{
    Fleet* incoming = battling_player->player->find_incoming(battling_player->player, sector);
    if (incoming) {
        int index = sector->incoming->index(sector->incoming, incoming);
        sector->incoming->remove(sector->incoming, index);
        incoming->destroy(incoming);
    }

    Fleet* in_place = battling_player->player->find_fleet(battling_player->player, sector);
    if (in_place) {
        battling_player->player->remove_fleet(battling_player->player, in_place);
        sector->fleet->destroy(sector->fleet);
        sector->fleet = NULL;
    }
}


/*
 * Removes a BattlingPlayer variable from the circular linked list, corresponding to a player having
 * lost a 1-vs-1 battle during a confrontation between more than two players.
 */
void _remove_defeated(CircularLinkedList* battling_players, BattlingPlayer* battling_player)
{
    int index = battling_players->index(battling_players, battling_player);
    free(battling_player);
    battling_players->remove_at(battling_players, index);
}


/*
 * Handles a battle between more than two players in the given sector. Returns the winner or NULL in
 * case the players battle to tie.
 */
Player* battle_between_more_than_two_players(Vector* players, Sector* sector, Galaxy* galaxy)
{
    CircularLinkedList* battling_players = get_battling_players(players, sector);
    if (!battling_players) {
        players->free(players);
        galaxy->destroy(galaxy);
        exit(EXIT_FAILURE);
    }

    DNode* node = battling_players->head;
    while (node && battling_players->size > 1) {
        BattlingPlayer* p_curr = node->data;
        BattlingPlayer* p_next = node->next->data;

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
            _remove_defeated(battling_players, p_curr);
        }
        if (p_next->power <= 0) {
            if (node->data == p_next)
                node = battling_players->size == 1 ? NULL : node->prev;
            _remove_defeated(battling_players, p_next);
        }

        if (node) node = node->next;
    }

    Player* winner;
    if (battling_players->size == 0) {
        battle_at_tie(sector);
        for (unsigned int i = 0; i < players->size; i++)
            sector->mark_at_tie(sector, players->data[i], galaxy);
        winner = NULL;
    } else {
        winner = ((BattlingPlayer*) battling_players->head->data)->player;
        free(battling_players->head->data);
    }

    battling_players->free(battling_players);
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
        notify_battle_header(players, sector);
        winner = battle_between_two_players(players, sector, galaxy);
    } else {
        shuffle(players->data, players->size); // random order for attack
        notify_battle_header(players, sector);
        winner = battle_between_more_than_two_players(players, sector, galaxy);
    }

    players->free(players);
    return winner;
}
