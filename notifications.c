//
// Created by Alexandra Zaharia on 28/12/18.
//

#include <stdio.h>
#include "notifications.h"
#include "player.h"

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
 * Notifies the player by how much the firepower increased in the sector.
 */
void notify_ships_built(Sector* sector, unsigned int power)
{
    printf(">>>\n>>> Your firepower in sector (%hu, %hu) has increased by %u.\n>>>\n",
           sector->x, sector->y, power);
    notification_header('#');
}


/*
 * Notifies the player that a planet has been colonized.
 */
void notify_planet_colonized(Sector* sector)
{
    printf("###\n### Congratulations! ");
    printf("The planet in sector (%hu, %hu) is now yours.\n", sector->x, sector->y);
    printf("### It produces %hu resources/turn.\n###\n", sector->planet->res_per_turn);
    notification_header('#');
}


/*
 * Notifies the player that a planet has been lost to an AI player.
 */
void notify_planet_lost(Planet* planet)
{
    printf("###\n### Calamity! ");
    printf("The AI conquered your planet in sector (%hu, %hu).\n###\n", planet->x, planet->y);
    notification_header('#');
}


/*
 * Notifies the human player that the newly explored sector generated bonus resources that have been
 * sent to the player's home planet.
 */
void notify_sector_explored(Sector* sector, Planet* home)
{
    printf("$$$\n$$$ Exploring sector (%hu, %hu) grants you %hu bonus resources!\n",
            sector->x, sector->y, sector->res_bonus);
    printf("$$$ Bonus resources have been added to your home planet in sector (%hu, %hu).\n$$$\n",
            home->x, home->y);
    notification_header('#');
}


/*
 * Displays a notification that the specified player has been eliminated from the game.
 */
void notify_player_eliminated(Player* player)
{
    printf("---\n--- Player %s%c%s has been eliminated.\n", player->color, player->symbol, RESET);
    printf("---\n");
    notification_header('#');
}


/*
 * Displays the game over notification.
 */
void notify_game_over(Galaxy* galaxy)
{
    printf("\n" BOLDWHITE);
    notification_header('*');
    printf("***\n");
    printf("*** GAME OVER\n***\n");

    Player* winner = galaxy->players->data[0];
    printf("*** Player %s%c%s conquered the galaxy in %u turns!\n***\n",
           winner->color, winner->symbol, BOLDWHITE, galaxy->turn);

    notification_header('*');
    printf(RESET);
}
