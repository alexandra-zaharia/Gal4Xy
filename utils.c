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

/*
 * Returns the index in the given list equal to item, or -1 in case the item is not in the list.
 */
int get_index_in_list(LinkedList* list, void* item)
{
    int index = -1;
    for (DNode* node = list->head; node; node = node->next) {
        ++index;
        if (node->data == item)
            return index;
    }
    return -1;
}
