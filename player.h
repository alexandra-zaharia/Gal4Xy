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
    char symbol;                           // symbol associated to the player
    char* color;                           // player color for galaxy display

    LinkedList* planets;                   // list of planets owned by the player
    LinkedList* fleets;                    // list of fleets owned by the player

    void (*play)(Player*, Galaxy*);        // player strategy for one turn
    Planet* (*home_planet)(Player*);       // returns the player's home planet (if any)
    Fleet* (*find_fleet)(Player*, unsigned short int, unsigned short int); // returns fleet at (x,y)
    void (*move_fleet)(Player*, Galaxy*, int, int, int, int, int);         // moves a fleet
    void (*update_resources)(Player*);     // updates resources for player's planets
    void (*build_ships)(Player*, Galaxy*); // builds ships for the player where applicable

    void (*destroy)(Player*);              // frees the player
};

Player* player_create(char symbol, char* color);

#endif //GAL4XY_PLAYER_H
