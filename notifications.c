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
 * Displays a notification informing the player by how much the firepower increased in the sector.
 */
void notify_ships_built(Sector* sector, unsigned int power)
{
    printf(">>>\n>>> Your firepower in sector (%hu, %hu) has increased by %u.\n>>>\n",
           sector->x, sector->y, power);
    notification_header('#');
}

/*
 * Displays a notification informing the player that a planet has been colonized.
 */
void notify_planet_colonized(Sector* sector)
{
    printf("###\n### Congratulations! ");
    printf("The planet in sector (%hu, %hu) is now yours.\n", sector->x, sector->y);
    printf("### It produces %hu resources/turn.\n###\n", sector->planet->res_per_turn);
    notification_header('#');
}

void notify_sector_explored(Sector* sector, Planet* home)
{
    printf("$$$\n$$$ Exploring sector (%hu, %hu) grants you %hu bonus resources!\n",
            sector->x, sector->y, sector->res_bonus);
    printf("$$$ Bonus resources have been added to your home planet in sector (%hu, %hu).\n$$$\n",
            home->x, home->y);
    notification_header('#');
}
