//
// Created by Alexandra Zaharia on 28/12/18.
//

#ifndef GAL4XY_NOTIFICATIONS_H
#define GAL4XY_NOTIFICATIONS_H

#include "sector.h"

void notification_header(char);

void notify_ships_built(Sector*, unsigned int);

void notify_planet_colonized(Sector*);
void notify_planet_lost(Planet*);

void notify_sector_explored(Sector*, Planet*);

void notify_battle_header(Sector*, Galaxy* galaxy);
void notify_battle_summary(Player*);

void notify_player_eliminated(Player*);
void notify_game_over(Galaxy*);


#endif //GAL4XY_NOTIFICATIONS_H
