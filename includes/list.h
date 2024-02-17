#ifndef __LIST__
#define __LIST__

#include <stdlib.h>
#include <assert.h>

#define INIT_CAPACITY 4

/**
 * this code come from this project : https://github.com/tsoding/musializer/blob/master/src/nob.h
 * made by https://github.com/tsoding
*/
#define list_append(list, item) \
do {                                                                                     \
    if ((list)->count >= (list)->capacity) {                                             \
        (list)->capacity = (list)->capacity == 0 ? INIT_CAPACITY : (list)->capacity*2;   \
        (list)->items = realloc((list)->items, (list)->capacity*sizeof(*(list)->items)); \
        assert((list)->items != NULL && "Buy more RAM lol");                             \
    }                                                                                    \
                                                                                         \
    (list)->items[(list)->count++] = (item);                                             \
} while (0)

#endif // __LIST__