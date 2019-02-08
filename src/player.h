//
// Created by Alexandra Zaharia on 26/12/18.
//

#ifndef GAL4XY_PLAYER_H
#define GAL4XY_PLAYER_H

#include "linked_list.h"
#include "galaxy.h"
#include "color.h"

typedef struct Player Player;

struct Player {
    char symbol;                             // symbol associated to the player
    char* color;                             // player color for galaxy display

    bool is_retired;                         // has the player been eliminated yet?
    unsigned int retirement_turn;            // turn when the player was eliminated

    Planet* home_planet;                     // player's home planet (if any)
    LinkedList* planets;                     // list of planets owned by the player
    LinkedList* fleets;                      // list of fleets owned by the player

    void (*add_fleet)(Player*, Fleet*);      // adds a fleet to the player's fleets
    void (*remove_fleet)(Player*, Fleet*);   // removes a fleet from the player's fleets
    void (*add_planet)(Player*, Planet*);    // adds a planet to the player's planets
    void (*remove_planet)(Player*, Planet*, Galaxy*); // removes a planet from the player's planets
    void (*reassign_home_planet)(Player*);   // reassign one of the player's planets as home planet

    Fleet* (*find_fleet)(Player*, Sector*);                        // fleet in place in sector
    Fleet* (*find_incoming)(Player*, Sector*);                     // incoming fleet in sector
    void (*move_fleet)(Player*, Galaxy*, int, int, int, int, int); // moves a fleet

    void (*update_resources)(Player*);       // updates resources for player's planets
    void (*build_ships)(Player*, Galaxy*);   // builds ships for the player where applicable

    void (*play)(Player*, Galaxy*);          // player strategy for one turn
    void (*destroy)(Player*);                // frees the player
};

Player* player_create(char symbol, char* color);

#endif //GAL4XY_PLAYER_H
