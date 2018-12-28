//
// Created by Alexandra Zaharia on 28/12/18.
//

#include <stdio.h>
#include "notifications.h"


/*
 * Displays a notification header with the specified symbol.
 */
void notification_header(char symbol)
{
    for (int i = 0; i < 80; i++)
        printf("%c", symbol);
    printf("\n");
}


/*
 * Displays a notification informing the player by how much the firepower increased in sector
 * (x, y).
 */
void notification_ships_built(unsigned short int x, unsigned short int y, unsigned int power)
{
    printf(">>>\n>>> Your firepower in sector (%hu, %hu) has increased by %hu.\n>>>\n",
           x, y, power);
    notification_header('#');
}
