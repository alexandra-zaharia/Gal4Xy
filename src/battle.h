//
// Created by Alexandra Zaharia on 06/01/19.
//

#ifndef GAL4XY_BATTLE_H
#define GAL4XY_BATTLE_H

#include "galaxy.h"

Vector* players_in_conflict(Sector*);
unsigned int total_firepower(Player*, Sector*);
Player* battle(Sector*, Galaxy*);

#endif //GAL4XY_BATTLE_H
