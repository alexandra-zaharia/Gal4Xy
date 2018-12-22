//
// Created by Alexandra Zaharia on 09/12/18.
//

#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "include/vector.h"
#include "galaxy.h"
#include "utils.h"
#include "io.h"

const char O_NONE = '?';
const char O_AI = '@';
const char O_HUMAN = '*';


/*
 * Determines the two home planets (for the human and AI players) in the galaxy.
 */
void home_planets_initialize(Galaxy* galaxy, Vector* planets)
{
    unsigned short int h_home;
    unsigned short int a_home;

    h_home = random_number(0, (unsigned short int)(planets->size - 1));
    do {
        a_home = random_number(0, (unsigned short int)(planets->size - 1));
    } while (h_home == a_home);

    galaxy->home_h = planets->data[h_home];
    galaxy->home_a = planets->data[a_home];

    ((Sector*) planets->data[h_home])->planet->owner = O_HUMAN;
    ((Sector*) planets->data[a_home])->planet->owner = O_AI;

    ((Sector*) planets->data[h_home])->explored_h = true;
    ((Sector*) planets->data[a_home])->explored_a = true;
}

/*
 * Initializes the galaxy by determining which sectors contain planets and which are empty. For
 * sectors containing planets, determine how many resources per turn they generate. For empty
 * sectors, determine their exploration bonus.
 */
void galaxy_initialize(Galaxy* galaxy)
{
    Vector* planets = vector_create();

    srand((unsigned int) time(NULL));

    for (unsigned short i = 0; i < SIZE; i++)
        for (unsigned short int j = 0; j < SIZE; j++) {
            Sector* sector = &galaxy->sectors[i][j];
            sector->explored_h = sector->explored_a = false;
            sector->x = i;
            sector->y = j;

            double random = (double)rand() / RAND_MAX;
            if (random <= PLANET_PROB) {
                sector->has_planet = true;
                planets->add(planets, sector);
                sector->planet = planet_create();
            } else {
                sector->has_planet = false;
                sector->res_bonus = (unsigned short int)
                    (random_number(BONUS_MIN, BONUS_MAX + BONUS_MIN - 1) / BONUS_MIN * BONUS_MIN);
            }
        }

    home_planets_initialize(galaxy, planets);
    planets->free(planets);
}

/*
 * Frees the galaxy.
 */
void galaxy_free(Galaxy* galaxy)
{
    if (!galaxy) return;

    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (galaxy->sectors[i][j].has_planet) {
                Planet* planet = galaxy->sectors[i][j].planet;
                planet->destroy(planet);
            }
    for (int i = 0; i < SIZE; i++)
        free(galaxy->sectors[i]);
    free(galaxy->sectors);
    free(galaxy);
}

/*
 * Creates and returns an empty galaxy (having SIZE x SIZE uninitialized sectors).
 */
Galaxy* galaxy_create()
{
    Galaxy* galaxy = malloc(sizeof(Galaxy));
    if (!galaxy) {
        MALLOC_ERROR(__func__);
        exit(EXIT_FAILURE);
    }
    galaxy->sectors = malloc(SIZE * sizeof(Sector*));
    if (!galaxy->sectors) {
        MALLOC_ERROR(__func__);
        free(galaxy);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < SIZE; i++) {
        galaxy->sectors[i] = malloc(SIZE * sizeof(Sector));
        if (!galaxy->sectors[i]) {
            MALLOC_ERROR(__func__);
            for (int j = 0; j < i; j++)
                free(galaxy->sectors[j]);
            free(galaxy->sectors);
            free(galaxy);
            exit(EXIT_FAILURE);
        }
    }

    galaxy->game_over = false;
    galaxy->turn = 0;
    galaxy->home_a = NULL;
    galaxy->home_h = NULL;

    galaxy->initialize = galaxy_initialize;
    galaxy->display = galaxy_display;
    galaxy->destroy = galaxy_free;

    return galaxy;
}
