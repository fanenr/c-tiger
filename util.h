#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <stdio.h>

/* ********************************************** */
/*                     error                      */
/* ********************************************** */

#define error(FMT, ...)                                                       \
  do                                                                          \
    {                                                                         \
      fprintf (stderr, "%s (%s:%d): ", __FILE__, __FUNCTION__, __LINE__);     \
      fprintf (stderr, FMT, ##__VA_ARGS__);                                   \
      fprintf (stderr, "\n");                                                 \
      quit ();                                                                \
    }                                                                         \
  while (0)

extern void quit (void) __attribute__ ((noreturn));

/* ********************************************** */
/*                    memory                      */
/* ********************************************** */

extern void *checked_alloc (size_t size);
extern void *checked_realloc (void *ptr, size_t size);

extern long checked_atol (const char *src) __attribute__ ((nonnull (1)));
extern double checked_atod (const char *src) __attribute__ ((nonnull (1)));
extern char *checked_strdup (const char *src) __attribute__ ((nonnull (1)));

/* ********************************************** */
/*                    vector                      */
/* ********************************************** */

#define VECTOR_INIT_CAP 8
#define VECTOR_EXPAN_RATIO 2

typedef struct vector vector;

struct vector
{
  size_t cap;
  size_t size;
  void **data;
};

extern void vector_init (vector *vec) __attribute__ ((nonnull (1)));
extern void vector_free (vector *vec) __attribute__ ((nonnull (1)));
extern bool vector_reserve (vector *vec, size_t cap)
    __attribute__ ((nonnull (1)));
extern void *vector_get (vector *vec, size_t pos)
    __attribute__ ((nonnull (1)));
extern bool vector_insert (vector *vec, size_t pos, void *node)
    __attribute__ ((nonnull (1)));
extern bool vector_push_back (vector *vec, void *node)
    __attribute__ ((nonnull (1)));
extern bool vector_erase (vector *vec, size_t pos)
    __attribute__ ((nonnull (1)));

/* ********************************************** */
/*                     stack                      */
/* ********************************************** */

typedef struct stack stack;

struct stack
{
  vector vec;
};

extern void stack_init (stack *stac) __attribute__ ((nonnull (1)));
extern void stack_free (stack *stac) __attribute__ ((nonnull (1)));
extern bool stack_reserve (stack *stac, size_t cap)
    __attribute__ ((nonnull (1)));
extern void *stack_top (stack *stac) __attribute__ ((nonnull (1)));
extern bool stack_pop (stack *stac) __attribute__ ((nonnull (1)));
extern bool stack_push (stack *stac, void *node) __attribute__ ((nonnull (1)));

#endif