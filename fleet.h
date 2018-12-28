//
// Created by Alexandra Zaharia on 27/12/18.
//

#ifndef GAL4XY_FLEET_H
#define GAL4XY_FLEET_H

typedef struct Player Player;

typedef struct Fleet Fleet;
struct Fleet {
    unsigned short int x;     // x-coordinate of this fleet's sector in the galaxy
    unsigned short int y;     // y-coordinate of this fleet's sector in the galaxy
    Player* owner;            // fleet owner
    unsigned int power;       // the fleet's total firepower

    void (*move)(Fleet*, unsigned short int, unsigned short int, unsigned int); // moves the fleet
    void (*destroy)(Fleet*);  // frees the fleet
};

Fleet* fleet_create(unsigned short int, unsigned short int, Player*, unsigned int);

#endif //GAL4XY_FLEET_H
