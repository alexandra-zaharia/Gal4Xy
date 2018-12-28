//
// Created by Alexandra Zaharia on 11/12/18.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include "galaxy.h"
#include "color.h"
#include "io.h"

/*
 * Reads at most n characters (newline included) into str. If present, the newline is removed.
 * Returns the characters read (newline excluded). Taken from "C Primer Plus" by Stephen Prata,
 * sixth edition (2013), listing 11.10.
 */
char* s_gets(char* str, int n)
{
    char* ret_val = fgets(str, n, stdin);
    if (ret_val) {
        int i = 0;
        while (str[i] != '\n' && str[i] != '\0')
            i++;
        if (str[i] == '\n') {
            str[i] = '\0';
        } else {
            while (getchar() != '\n')
                continue;
        }
    }
    return ret_val;
}


/*
 * Reads and returns one character. If more than one character is provided, the extra characters
 * are removed.
 */
char s_getc()
{
    char* str = malloc(2);
    char* read = s_gets(str, 2);
    char c = read[0];
    free(str);
    return c;
}


void prompt_move_ships(Player* player, Galaxy* galaxy)
{
    int sx, sy, tx, ty, n;
    bool read_five_values = false;

    printf("\nFrom which sector, to which sector, and how many fleet units "
           "do you wish to move?\n");
    printf("e.g. To move a firepower of 8 from sector (1, 1) to sector (2, 2), "
           "type 1 1 2 2 8.\n");

    char* buffer = malloc(21);
    char* move_command = s_gets(buffer, 21);
    char* token = strtok(move_command, " ");
    int count = 0;

    while(token) {
        if (++count > 5) break;

        char* end;
        long value = strtol(token, &end, 10);
        if (end == token) {
            fprintf(stderr, "Not a number: '%s'\n", token);
            break;
        } else if ((value == LONG_MIN || value == LONG_MAX) && errno) {
            fprintf(stderr, "Value out of (long) range: '%s'\n", token);
            break;
        } else if (value < INT_MIN || value > INT_MAX) {
            fprintf(stderr, "Value out of (int) range: %ld\n", value);
            break;
        } else {
            switch (count) {
                case 1: sx = (int) value; break;
                case 2: sy = (int) value; break;
                case 3: tx = (int) value; break;
                case 4: ty = (int) value; break;
                case 5: {
                    n = (int) value;
                    read_five_values = true;
                }; break;
                default: break;
            }
        }

        token = strtok(NULL, " ");
    }

    free(buffer);

    if (read_five_values) player->move_fleet(player, galaxy, sx, sy, tx, ty, n);
}


void find_fleets(Player* player)
{
    if (!player->fleets->size) {
        printf("\nYou have no fleet in the whole wide galaxy. :-(\n");
        printf("Or maybe you do but they're all in transit. :-)\n");
        return;
    }

    printf("You have %d fleet%s:\n", player->fleets->size, player->fleets->size > 1 ? "s" : "");
    for (DNode* node = player->fleets->head; node; node = node->next) {
        Fleet* fleet = (Fleet*) node->data;
        printf("\tSector (%hu, %hu) has a fleet with %u total firepower.\n",
                fleet->x, fleet->y, fleet->power);
    }
}


void find_planets(Player* player)
{
    printf("You have %d planet%s:\n", player->planets->size, player->planets->size > 1 ? "s" : "");
    for (DNode* node = player->planets->head; node; node = node->next) {
        Planet* planet = (Planet*) node->data;
        printf("\tPlanet in sector (%hu, %hu):\n", planet->x, planet->y);
        printf("\t\tResources per turn: %hu\n", planet->res_per_turn);
        printf("\t\tTotal resources: %hu\n", planet->res_total);
    }
}


void cheat(Galaxy*);


void prompt(Player* player, Galaxy* galaxy)
{
    char option;

    do {
        printf("\nYou have many options!\n");
        printf("\tm - move fleets\n");
        printf("\tf - find your available fleets\n");
        printf("\tp - show your planets\n");
        printf("\tg - show galaxy\n");
        printf("\tt - end turn\n");
        printf("\tq - quit\n");
        printf("What would you like to do? ");
        option = s_getc();
        switch (option) {
            case 'c':
            case 'C': cheat(galaxy); break;
            case 'm':
            case 'M': prompt_move_ships(player, galaxy); break;
            case 'f':
            case 'F': find_fleets(player); break;
            case 'p':
            case 'P': find_planets(player); break;
            case 'g':
            case 'G': galaxy->display(galaxy); break;
            case 't':
            case 'T': break;
            case 'q':
            case 'Q': {
                printf("Are you sure you want to quit? (y/n) ");
                char quit = s_getc();
                while (quit != 'y' && quit != 'Y' && quit != 'n' && quit != 'N') {
                    printf("Please enter 'y' or 'n': ");
                    quit = s_getc();
                }
                if (quit == 'y' || quit == 'Y') {
                    galaxy->game_over = true;
                }
            }; break;
            default: printf("Invalid option '%c'\n", option);
        }
    } while (option != 't' && option != 'T' && option != 'q' && option != 'Q');

    printf("\n");
}


void display_greeting(Galaxy* galaxy, bool cheat)
{
    if (galaxy->turn == 0) {
        Player* human = (Player*) galaxy->players->data[0];
        int len_greeting = printf(
                BOLDWHITE "Welcome to this galaxy! Your home planet is in sector "
                "%s(%u, %u)" BOLDWHITE ".\n" RESET,
                human->color, human->home_planet(human)->x, human->home_planet(human)->y);
        if (cheat) {
            for (unsigned int i = 1; i < galaxy->players->size; i++) {
                Player *ai = (Player *) galaxy->players->data[i];
                printf("The AI home planet is in sector %s(%u, %u)" BOLDWHITE ".\n" RESET,
                       ai->color, ai->home_planet(ai)->x, ai->home_planet(ai)->y);
            }
        }
        len_greeting -= (int) (2 * strlen(BOLDWHITE) + strlen(human->color) + strlen(RESET));
        for (int i = 0; i < len_greeting - 1; i++)
            printf("=");
        printf("\n\n");
    } else {
        printf("\n");
    }
}


void log_turn(Galaxy* galaxy)
{
    printf("Turn #%u\n\n", galaxy->turn);
}


void display_indexes()
{
    for (unsigned short int i = 0; i < SIZE; i++) {
        if (i == 0)
            printf("    ");
        printf(" %hu  ", i);
    }
    printf("\n");
}


void display_separator()
{
    for (unsigned short int j = 0; j < SIZE; j++) {
        if (j == 0)
            printf("   +");
        printf("---+");
    }
    printf("\n");
}


void display_sectors(Galaxy* galaxy, bool cheat)
{
    for (unsigned short int i = 0; i < SIZE; i++) {
        for (unsigned short int j = 0; j < SIZE; j++) {
            Sector* sector = galaxy->sectors[i][j];

            if (j == 0)
                printf(" %hu |", i);
            char symbol;
            char* color_prefix = "";
            char* color_suffix = "";
            if (cheat) {
                if (sector->has_planet) {
                    if (sector->planet->owner) {
                        symbol = sector->planet->owner->symbol;
                        color_prefix = sector->planet->owner->color;
                    } else {
                        symbol = O_NONE;
                        color_prefix = YELLOW;
                    }
                    color_suffix = RESET;
                } else {
                    symbol = (char) ' ';
                }
            } else {
                symbol = (bool) sector->explored->data[0]
                        ? sector->has_planet ? sector->planet->owner->symbol : (char) ' '
                        : O_NONE;
            }
            if (sector->explored->data[0]) {
                color_prefix = sector->planet->owner->color;
                color_suffix = RESET;
            }
            printf(" %s%c%s |", color_prefix, symbol, color_suffix);
        }
        printf("\n");
        display_separator();
    }
}


void galaxy_display(Galaxy* galaxy)
{
    display_greeting(galaxy, false);
    log_turn(galaxy);
    display_indexes();
    display_separator();
    display_sectors(galaxy, false);
}


void cheat(Galaxy *galaxy)
{
    display_greeting(galaxy, true);
    log_turn(galaxy);
    display_indexes();
    display_separator();
    display_sectors(galaxy, true);
}


