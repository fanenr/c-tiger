#ifndef ARRAY_EXT_H
#define ARRAY_EXT_H

#include "array.h"

typedef void array_visit_t (void *e);
typedef int array_comp_t (const void *a, const void *b);

extern void *array_find (const array_t *arr, const void *target,
                         array_comp_t *comp) attr_nonnull (1, 2, 3);

extern void array_for_each (array_t *arr, array_visit_t *visit)
    attr_nonnull (1, 2);

#endif
