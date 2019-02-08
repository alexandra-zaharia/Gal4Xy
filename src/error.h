//
// Created by Alexandra Zaharia on 27/12/18.
//

#ifndef GAL4XY_ERROR_H
#define GAL4XY_ERROR_H

#include <stdio.h>

#define MALLOC_ERROR(func, msg) fprintf(stderr, "%s: %s\n", (func), (msg))
#define INSUFFICIENT_PLANETS_ERROR(func, planets, players) \
    fprintf(stderr, "%s: insufficient planets (%d) for all players (%d)\n", \
    (func), (planets), (players))
#define INVALID_COLOR_ERROR(func, color) fprintf(stderr, "%s: invalid color (%s)\n", \
    (func), (color));

#define SECTOR_COORD_NEGATIVE_ERROR \
    fprintf(stderr, "Sector coordinates cannot be negative.\n")
#define SECTOR_COORD_TOO_BIG_ERROR \
    fprintf(stderr, "Sector coordinates cannot be greater than %d.\n", SIZE - 1)
#define FLEET_POWER_ERROR \
    fprintf(stderr, "You must move at least 1 firepower.\n")
#define SECTOR_FROM_TO_ERROR \
    fprintf(stderr, "The 'from' and 'to' sectors cannot be the same.\n")
#define NOT_FLEET_OWNER_ERROR(x, y) \
    fprintf(stderr, "You do not have a fleet in sector (%hu, %hu).\n", (x), (y))
#define FLEET_TOO_SMALL_ERROR(x, y) \
    fprintf(stderr, "You do not have enough firepower in sector (%hu, %hu).\n", (x), (y))

#endif //GAL4XY_ERROR_H
