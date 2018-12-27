//
// Created by Alexandra Zaharia on 26/12/18.
//

#ifndef GAL4XY_PLAYER_H
#define GAL4XY_PLAYER_H

#include "include/linked_list.h"
#include "galaxy.h"
#include "color.h"

typedef struct Player Player;

struct Player {
    char symbol;                     // symbol associated to the player
    char* color;                     // player color for galaxy display
    LinkedList* planets;             // list of planets owned by the player
    LinkedList* fleets;              // list of fleets owned by the player

    void (*play)(Player*, Galaxy*);  // player strategy for one turn
    Planet* (*home_planet)(Player*); // returns the player's home planet (if any)

    void (*destroy)(Player*);        // frees the player
};

Player* player_create(char symbol, char* color);

#endif //GAL4XY_PLAYER_H
