//
// Created by Alexandra Zaharia on 26/12/18.
//

#include <stdlib.h>
#include <stdio.h> // TODO
#include <math.h>
#include "ai.h"
#include "error.h"
#include "utils.h"
#include "io.h"


/*
 * Returns a vector of Sector* containing all the sectors in the galaxy that the player has not yet
 * explored.
 */
Vector* find_unexplored_sectors(Player* player, Galaxy* galaxy)
{
    Vector* unexplored = vector_create();
    if (!unexplored) {
        MALLOC_ERROR(__func__, "cannot create unexplored sectors vector");
        galaxy->destroy(galaxy);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (!galaxy->sectors[i][j]->is_explored(galaxy->sectors[i][j], player, galaxy))
                unexplored->add(unexplored, galaxy->sectors[i][j]);

    return unexplored;
}


typedef struct {
    Fleet* fleet;
    Sector* target;
} FleetDeployment;


/*
 * Deploys the player's fleets according to the fleet deployment `planner'.
 */
void deploy_fleet(Player* player, Galaxy* galaxy, LinkedList* planner)
{
    for (DNode* node = planner->head; node; node = node->next) {
        FleetDeployment* fd = (FleetDeployment*) node->data;
        player->move_fleet(
                player, galaxy, fd->fleet->x, fd->fleet->y, fd->target->x, fd->target->y, 1);
    }
}


/*
 * Plans the deployment of the player's `fleet' to a randomly chosen unexplored sector by adding
 * FleetDeployment variables to the `planner', if and only if it is possible to leave `keep' ships
 * in the original fleet, in case the original fleet is in a sector where the player owns a planet.
 */
void plan_fleet_deployment(
        Fleet* fleet,
        Vector* unexplored,
        unsigned int keep,
        Galaxy* galaxy,
        LinkedList* planner)
{
    for (unsigned  int i = fleet->power; i > 0; i--) {
        if (fleet->power <= keep && galaxy->sectors[fleet->x][fleet->y]->has_planet)
            break;
        unsigned short rnd = random_number(0, (unsigned short) (unexplored->size - 1));
        Sector* target = unexplored->data[rnd];
        FleetDeployment* fd = malloc(sizeof(FleetDeployment));
        if (!fd) {
            MALLOC_ERROR(__func__, "cannot create fleet deployment variable");
            planner->free(planner);
            unexplored->free(unexplored);
            galaxy->destroy(galaxy);
            exit(EXIT_FAILURE);
        }
        fd->fleet = fleet;
        fd->target = target;
        planner->insert_end(planner, fd);
    }
}


void ai_strategy(Player* player, Galaxy* galaxy) {
    Vector* unexplored = find_unexplored_sectors(player, galaxy);
    float explored_ratio = roundf((float) (SIZE*SIZE - unexplored->size) / (SIZE*SIZE) * 100) / 100;

    printf("%s for AI player '%c' with an exploration ratio of %.2f\n",
           __func__, player->symbol, explored_ratio);

    if (unexplored->size > 0) { // explore uncharted territory
        LinkedList *deployment_planner = linked_list_create();

        for (DNode *node = player->fleets->head; node; node = node->next) {
            Fleet *fleet = node->data;
            if (explored_ratio < 0.25) {
                plan_fleet_deployment(fleet, unexplored, 0, galaxy, deployment_planner);
            } else if (explored_ratio < 0.50) {
                plan_fleet_deployment(fleet, unexplored, 2, galaxy, deployment_planner);
            } else if (explored_ratio < 0.75) {
                plan_fleet_deployment(fleet, unexplored, 4, galaxy, deployment_planner);
            } else {
                plan_fleet_deployment(fleet, unexplored, 6, galaxy, deployment_planner);
            }
        }

        deploy_fleet(player, galaxy, deployment_planner);

        for (DNode *node = deployment_planner->head; node; node = node->next) {
            FleetDeployment *fd = node->data;
            free(fd);
        }
        deployment_planner->free(deployment_planner);
    } // else regain lost colonies

    unexplored->free(unexplored);
}
