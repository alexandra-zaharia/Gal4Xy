//
// Created by Alexandra Zaharia on 27/12/18.
//

#ifndef GAL4XY_ERROR_H
#define GAL4XY_ERROR_H

#define MALLOC_ERROR(func, msg) fprintf(stderr, "%s: %s\n", (func), (msg))
#define INSUFFICIENT_PLANETS_ERROR(func, planets, players) \
    fprintf(stderr, "%s: insufficient planets (%d) for all players (%d)\n", \
    (func), (planets), (players))
#define INVALID_COLOR_ERROR(func, color) fprintf(stderr, "%s: invalid color (%s)\n", \
    (func), (color));

#include <stdio.h>

#endif //GAL4XY_ERROR_H
