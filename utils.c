//
// Created by azaharia on 11/12/18.
//

#include <stdlib.h>
#include "utils.h"

/*
 * Returns a random (unsigned short) integer between min and max, inclusive.
 */
unsigned short int random_number(unsigned short int min, unsigned short int max)
{
    return (unsigned short int)(min + rand() % (max - min + 1));
}