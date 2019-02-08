//
// Created by Alexandra Zaharia on 11/12/18.
//

#ifndef GAL4XY_IO_H
#define GAL4XY_IO_H

#include "galaxy.h"
#include "player.h"

char* s_gets(char*, int);
char s_getc();
void prompt(Player*, Galaxy*);

void galaxy_display(Galaxy*);

void log_move_fleet(unsigned short int, unsigned short int,
                    unsigned short int, unsigned short int, unsigned int);

#endif //GAL4XY_IO_H
