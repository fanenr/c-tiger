#ifndef ARRAY_H
#define ARRAY_H

#include "util.h"
#include <stddef.h>

typedef struct array_t array_t;

struct array_t
{
  void *data;
  size_t cap;
  size_t size;
  size_t elem_size;
};

#define ARRAY_INIT                                                            \
  (array_t) {}

#define array_at(arr, pos)                                                    \
  ((pos) < (arr)->size ? (arr)->data + (pos) * (arr)->elem_size : NULL)

extern void *array_insert (array_t *arr, size_t pos) attr_nonnull (1);

extern void array_erase (array_t *arr, size_t pos) attr_nonnull (1);

extern void *array_push_back (array_t *arr) attr_nonnull (1);

extern void array_pop_back (array_t *arr) attr_nonnull (1);

#endif
