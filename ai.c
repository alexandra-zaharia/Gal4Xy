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
 * Plans the deployment of the player's `fleet' to a randomly chosen sector among the target sectors
 * by adding FleetDeployment variables to the `planner', if and only if it is possible to leave
 * `keep' ships in the original fleet, in case the original fleet is in a sector where the player
 * owns a planet.
 */
void plan_fleet_deployment(
        Fleet* fleet,
        Vector* targets,
        unsigned int keep,
        Galaxy* galaxy,
        LinkedList* planner)
{
    for (unsigned  int i = fleet->power; i > 0; i--) {
        if (fleet->power <= keep && galaxy->sectors[fleet->x][fleet->y]->has_planet)
            break;
        unsigned short rnd = random_number(0, (unsigned short) (targets->size - 1));
        Sector* target = (Sector*) targets->data[rnd];
        FleetDeployment* fd = fleet_deployment_create(fleet, target);
        if (!fd) {
            planner->free(planner);
            targets->free(targets);
            galaxy->destroy(galaxy);
            exit(EXIT_FAILURE);
        }
        planner->insert_end(planner, fd);
    }
}


/*
 * Deploys all player fleets according to the fleet deployment planner. Frees the planner when
 * finished.
 */
void deploy_fleets_as_planned(Player* player, Galaxy* galaxy, LinkedList* planner)
{
    deploy_fleet(player, galaxy, planner);

    for (DNode *node = planner->head; node; node = node->next) {
        FleetDeployment *fd = node->data;
        free(fd);
    }

    planner->free(planner);
}


/*
 * The specified player explores uncharted territory by sending out fleets throughout the galaxy.
 */
void explore_uncharted_territory(Player* player, Vector* unexplored, Galaxy* galaxy)
{
    LinkedList* deployment_planner = linked_list_create();
    if (!deployment_planner) {
        MALLOC_ERROR(__func__, "cannot create deployment planner linked list");
        unexplored->free(unexplored);
        galaxy->destroy(galaxy);
        exit(EXIT_FAILURE);
    }
    float explored_ratio = roundf((float) (SIZE*SIZE - unexplored->size) / (SIZE*SIZE) * 100) / 100;

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

    deploy_fleets_as_planned(player, galaxy, deployment_planner);
}


/*
 * Returns a vector of Sector* containing all the sectors in the galaxy that contain planets that
 * were previously owned by the player but that were subsequently lost to other players.
 */
Vector* find_lost_colonies(Player* player, Galaxy* galaxy) {
    Vector* lost_colonies = vector_create();
    if (!lost_colonies) {
        MALLOC_ERROR(__func__, "cannot create lost planets vector");
        return NULL;
    }

    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++) {
            Sector* sector = galaxy->sectors[i][j];
            if (sector->is_explored(sector, player, galaxy)
                    && sector->has_planet && sector->planet->owner != player) {
                lost_colonies->add(lost_colonies, sector);
            }
        }

    return lost_colonies;
}


/*
 * The specified player reclaims colonies that have been lost to other players.
 */
void reclaim_lost_colonies(Player* player, Galaxy* galaxy)
{
    LinkedList* deployment_planner = linked_list_create();
    if (!deployment_planner) {
        MALLOC_ERROR(__func__, "cannot create deployment planner linked list");
        galaxy->destroy(galaxy);
        exit(EXIT_FAILURE);
    }

    Vector* lost_colonies = find_lost_colonies(player, galaxy);
    if (!lost_colonies) {
        deployment_planner->free(deployment_planner);
        galaxy->destroy(galaxy);
        exit(EXIT_FAILURE);
    }

    for (DNode *node = player->fleets->head; node; node = node->next) {
        Fleet *fleet = node->data;
        plan_fleet_deployment(fleet, lost_colonies, 6, galaxy, deployment_planner);
    }

    deploy_fleets_as_planned(player, galaxy, deployment_planner);
    lost_colonies->free(lost_colonies);
}


/*
 * Simple AI strategy in which the given player prioritizes exploration. No attempt is yet made to
 * recapture colonies that have been conquered by other players.
 */
void ai_strategy(Player* player, Galaxy* galaxy) {
    if (player->is_retired) return;

    Vector* unexplored = find_unexplored_sectors(player, galaxy);

    if (unexplored->size > 0) {
        explore_uncharted_territory(player, unexplored, galaxy);
    } else {
        reclaim_lost_colonies(player, galaxy);
    }

    unexplored->free(unexplored);
}
