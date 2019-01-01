//
// Created by azaharia on 11/12/18.
//

#ifndef GAL4XY_UTILS_H
#define GAL4XY_UTILS_H

#include "player.h"
#include "include/linked_list.h"

unsigned short int random_number(unsigned short int min, unsigned short int max);
int get_index_in_list(LinkedList*, void*);
bool has_incoming_fleet(Sector*, Player*);

#endif //GAL4XY_UTILS_H
