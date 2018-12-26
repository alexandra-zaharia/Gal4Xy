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
 * Determines whether an array of unspecified size contains the specified value before a given
 * index.
 */
bool has_value_before_index(
        unsigned short int const array[], unsigned short int value, unsigned short int index) {
    for (unsigned short int i = 0; i < index; i ++) {
        if (array[i] == value)
            return true;
    }
    return false;
}

/*
 * Determines the two home planets (for the human and AI players) in the galaxy.
 */
void home_planets_initialize(Galaxy* galaxy, Vector* planets)
{
    if (planets->size < galaxy->players->size) {
        fprintf(stderr, "%s: insufficient planets (%d) for all players (%d)\n",
                __func__, planets->size, galaxy->players->size);
        galaxy->destroy(galaxy);
    }

    unsigned short int home_planets[galaxy->players->size];

    short unsigned int i;
    for (i = 0; i < galaxy->players->size; i++) {
        do {
            home_planets[i] = random_number(0, (unsigned short int) (planets->size - 1));
        } while (has_value_before_index(home_planets, home_planets[i], i));
    }

    for (i = 0; i < galaxy->players->size; i++) {
        Player* player = (Player*) galaxy->players->data[i];
        player->planets->insert_start(player->planets, planets->data[home_planets[i]]);
        Planet* home_planet = (Planet*) player->planets->head->data;
        home_planet->owner = player->symbol;
        printf("Assigned %c to planet in %hu, %hu\n",
                home_planet->owner, home_planet->x, home_planet->y);
//        Sector* sector = &galaxy->sectors[home_planet->x][home_planet->y];
//        sector->explored->data[i] = (void*) true;
    }
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

    for (unsigned short int i = 0; i < SIZE; i++)
        for (unsigned short int j = 0; j < SIZE; j++) {
            Sector* sector = &galaxy->sectors[i][j];
            sector->explored_h = sector->explored_a = false;
//            sector->explored = vector_create();
//            if (!sector->explored) {
//                MALLOC_ERROR(__func__);
//                galaxy->destroy(galaxy);
//                exit(EXIT_FAILURE);
//            }
            //sector->explored_h = sector->explored_a = false;
            sector->x = i;
            sector->y = j;

            double random = (double)rand() / RAND_MAX;
            if (random <= PLANET_PROB) {
                sector->has_planet = true;
                planets->add(planets, sector);
                sector->planet = planet_create(i, j);
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
                Sector* sector = &galaxy->sectors[i][j];
                sector->planet->destroy(sector->planet);
//                sector->explored->free(sector->explored);
            }

    for (int i = 0; i < SIZE; i++)
        free(galaxy->sectors[i]);
    free(galaxy->sectors);

//    for (unsigned int i = 0; i < galaxy->players->size; i++) {
//        Player* player = (Player*) galaxy->players->data[i];
//        player->destroy(player);
//    }
//    galaxy->players->free(galaxy->players);

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

//    galaxy->players = vector_create();
//    if (!galaxy->players) {
//        MALLOC_ERROR(__func__);
//        free(galaxy);
//        exit(EXIT_FAILURE);
//    }
    galaxy->players = NULL;

    galaxy->sectors = malloc(SIZE * sizeof(Sector*));
    if (!galaxy->sectors) {
        MALLOC_ERROR(__func__);
//        galaxy->players->free(galaxy->players);
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
//            galaxy->players->free(galaxy->players);
            free(galaxy);
            exit(EXIT_FAILURE);
        }
    }

    galaxy->game_over = false;
    galaxy->turn = 0;

    galaxy->initialize = galaxy_initialize;
    galaxy->display = galaxy_display;
    galaxy->destroy = galaxy_free;

    return galaxy;
}
