//
// Created by Alexandra Zaharia on 11/12/18.
//

#include <string.h>
#include "galaxy.h"
#include "color.h"

void display_greeting(Galaxy* galaxy)
{
    if (galaxy->turn == 0) {
        int len_greeting = printf(
                BOLDWHITE "Welcome to this galaxy! Your home planet is in sector "
                BOLDGREEN "(%u, %u)" BOLDWHITE ".\n" RESET, galaxy->home_h->x, galaxy->home_h->y);
        len_greeting -= strlen(BOLDWHITE) * 2 + strlen(BOLDGREEN) + strlen(RESET);
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

void display_sectors(Galaxy* galaxy)
{
    display_indexes();
    display_separator();

    for (unsigned short int i = 0; i < SIZE; i++) {
        for (unsigned short int j = 0; j < SIZE; j++) {
            if (j == 0)
                printf(" %hu |", i);
            char symbol = galaxy->sectors[i][j].explored_h ? galaxy->sectors[i][j].has_planet
                          ? galaxy->sectors[i][j].planet.owner : (char) ' '
                          : (char) '?';
            char* bold_prefix = "";
            char* bold_suffix = "";
            if (galaxy->sectors[i][j].explored_h) {
                bold_prefix = BOLDGREEN;
                bold_suffix = RESET;
            }
            printf(" %s%c%s |", bold_prefix, symbol, bold_suffix);
        }
        printf("\n");
        display_separator();
    }
}

void galaxy_display(Galaxy* galaxy)
{
    display_greeting(galaxy);
    log_turn(galaxy);
    display_sectors(galaxy);
}
