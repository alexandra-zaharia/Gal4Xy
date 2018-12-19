//
// Created by azaharia on 11/12/18.
//

#ifndef GAL4XY_UTILS_H
#define GAL4XY_UTILS_H

#include <stdio.h>

#define MALLOC_ERROR(x) fprintf(stderr, "%s: insufficient memory\n", (x))

unsigned short int random_number(unsigned short int min, unsigned short int max);

#endif //GAL4XY_UTILS_H
