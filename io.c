//
// Created by Alexandra Zaharia on 11/12/18.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "galaxy.h"
#include "color.h"

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


void prompt_move_ships()
{
    printf("%s: not implemented\n", __func__);
}


void find_fleets()
{
    printf("%s: not implemented\n", __func__);
}


void find_planets()
{
    printf("%s: not implemented\n", __func__);
}


void prompt(Galaxy* galaxy) {
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
            case 'm':
            case 'M': prompt_move_ships(); break;
            case 'f':
            case 'F': find_fleets(); break;
            case 'p':
            case 'P': find_planets(); break;
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
                    galaxy->destroy(galaxy);
                    exit(EXIT_SUCCESS);
                }
            }; break;
            default: printf("Invalid option '%c'\n", option);
        }
    } while (option != 't' && option != 'T');

    printf("\n");
}


void display_greeting(Galaxy* galaxy)
{
    if (galaxy->turn == 0) {
        int len_greeting = printf(
                BOLDWHITE "Welcome to this galaxy! Your home planet is in sector "
                BOLDGREEN "(%u, %u)" BOLDWHITE ".\n" RESET, galaxy->home_h->x, galaxy->home_h->y);
        len_greeting -= 2 * strlen(BOLDWHITE) + strlen(BOLDGREEN) + strlen(RESET);
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
    for (unsigned short int i = 0; i < SIZE; i++) {
        for (unsigned short int j = 0; j < SIZE; j++) {
            if (j == 0)
                printf(" %hu |", i);
            char symbol = galaxy->sectors[i][j].explored_h ? galaxy->sectors[i][j].has_planet
                          ? galaxy->sectors[i][j].planet->owner : (char) ' '
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
    display_indexes();
    display_separator();
    display_sectors(galaxy);
}
