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
#include "error.h"
#include "sector.h"

const char O_NONE = '?';


/*
 * Determines whether an array of unspecified size contains the specified value before a given
 * index.
 */
bool value_already_present_before_index(
        const unsigned short *array, unsigned short int value, unsigned short int index) {
    for (unsigned short int i = 0; i < index; i ++) {
        if (array[i] == value)
            return true;
    }
    return false;
}


/*
 * Determines the two home planets (for the human and AI players) in the galaxy.
 *
 * Returns true if home planets were correctly initialized, false otherwise.
 */
bool home_planets_initialize(Galaxy* galaxy, Vector* planets)
{
    if (planets->size < galaxy->players->size) {
        INSUFFICIENT_PLANETS_ERROR(__func__, planets->size, galaxy->players->size);
        return false;
    }

    unsigned short int home_planets[galaxy->players->size];

    unsigned short int i;
    for (i = 0; i < galaxy->players->size; i++) {
        do {
            home_planets[i] = random_number(0, (unsigned short int) (planets->size - 1));
        } while (value_already_present_before_index(home_planets, home_planets[i], i));
    }

    for (i = 0; i < galaxy->players->size; i++) {
        Player* player = (Player*) galaxy->players->data[i];
        player->planets->insert_start(player->planets, planets->data[home_planets[i]]);
        Planet* home_planet = (Planet*) player->planets->head->data;
        home_planet->owner = player;
        Sector* sector = galaxy->sectors[home_planet->x][home_planet->y];
        sector->explored->data[i] = (void*) true;
    }

    return true;
}


/*
 * Initializes the galaxy by determining which sectors contain planets and which are empty. For
 * sectors containing planets, determine how many resources per turn they generate. For empty
 * sectors, determine their exploration bonus. Each sector also stores an explored state for every
 * player.
 *
 * Returns true if the galaxy was correctly initialized, false otherwise.
 */
bool galaxy_initialize(Galaxy* galaxy, Vector* players)
{

    galaxy->players = players;
    Vector* planets = vector_create();
    if (!planets) {
        MALLOC_ERROR(__func__, "cannot create planets vector");
        return false;
    }

    bool status = true;
    srand((unsigned int) time(NULL));

    for (unsigned short int i = 0; i < SIZE; i++)
        for (unsigned short int j = 0; j < SIZE; j++) {
            Sector* sector = galaxy->sectors[i][j];
            sector->explored = vector_create();
            if (!sector->explored) {
                MALLOC_ERROR(__func__, "cannot create explored vector");
                status = false;
                goto free_planets;
            }

            for (unsigned int k = 0; k < players->size; k++)
                sector->explored->add(sector->explored, false);

            sector->x = i;
            sector->y = j;

            double random = (double)rand() / RAND_MAX;
            if (random <= PLANET_PROB) {
                sector->planet = planet_create(i, j);
                if (!sector->planet) {
                    status = false;
                    goto free_planets;
                }
                sector->has_planet = true;
                planets->add(planets, sector->planet);
            } else {
                sector->has_planet = false;
                sector->res_bonus = (unsigned short int)
                    (random_number(BONUS_MIN, BONUS_MAX + BONUS_MIN - 1) / BONUS_MIN * BONUS_MIN);
            }
        }

    status = status && home_planets_initialize(galaxy, planets);

free_planets:
    planets->free(planets);

    return status;
}


/*
 * Updates resources for every planet owned by a player.
 */
void update_resources(Galaxy* galaxy)
{
    for (unsigned int i = 0; i < galaxy->players->size; i++) {
        Player* player = (Player*) galaxy->players->data[i];
        player->update_resources(player);
    }
}


/*
 * Updates the galaxy by advancing one turn: planet resources are gathered for planets owned by
 * players, ships are built and moved, sectors are explored and battles take place where applicable.
 */
void galaxy_update(Galaxy* galaxy)
{
    if (++galaxy->turn > 1) {
        update_resources(galaxy);
    }
}


/*
 * Frees (de-allocates) the galaxy.
 */
void galaxy_free(Galaxy* galaxy)
{
    if (!galaxy) return;

    if (galaxy->sectors) {
        for (int i = 0; i < SIZE; i++) {
            if (!galaxy->sectors[i]) continue;
            for (int j = 0; j < SIZE; j++) {
                Sector *sector = galaxy->sectors[i][j];
                if (sector) {
                    sector->destroy(sector);
                }
            }
        }

        for (int i = 0; i < SIZE; i++)
            if (galaxy->sectors[i])
                free(galaxy->sectors[i]);

        free(galaxy->sectors);
    }

    if (galaxy->players) {
        for (unsigned int i = 0; i < galaxy->players->size; i++) {
            Player *player = (Player *) galaxy->players->data[i];
            player->destroy(player);
        }
        galaxy->players->free(galaxy->players);
    }

    free(galaxy);
}


/*
 * Creates and returns an empty galaxy (having SIZE x SIZE uninitialized sectors).
 *
 * Returns NULL in case of failure.
 */
Galaxy* galaxy_create()
{
    Galaxy* galaxy = malloc(sizeof(Galaxy));
    if (!galaxy) {
        MALLOC_ERROR(__func__, "cannot create galaxy");
        return NULL;
    }

    galaxy->initialize = galaxy_initialize;
    galaxy->display = galaxy_display;
    galaxy->update = galaxy_update;
    galaxy->destroy = galaxy_free;

    galaxy->players = NULL;
    galaxy->game_over = false;
    galaxy->turn = 0;

    galaxy->sectors = malloc(SIZE * sizeof(Sector**));
    if (!galaxy->sectors) {
        MALLOC_ERROR(__func__, "cannot create sectors");
        galaxy->destroy(galaxy);
        return NULL;
    }

    for (unsigned short int i = 0; i < SIZE; i++) {
        galaxy->sectors[i] = malloc(SIZE * sizeof(Sector*));
        if (!galaxy->sectors[i]) {
            MALLOC_ERROR(__func__, "cannot create sector arrays");
            for (int j = 0; j < i; j++)
                free(galaxy->sectors[j]);
            free(galaxy->sectors);
            free(galaxy);
            return NULL;
        }
        for (unsigned short int j = 0; j < SIZE; j++)
            galaxy->sectors[i][j] = sector_create(i, j);
    }

    return galaxy;
}
