#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>

/* **************************************************************** */
/*                              error                               */
/* **************************************************************** */

#define error(FMT, ...)                                                       \
  do                                                                          \
    {                                                                         \
      fprintf (stderr, "%s:%d (%s): ", __FUNCTION__, __LINE__, __FILE__);     \
      fprintf (stderr, FMT, ##__VA_ARGS__);                                   \
      fprintf (stderr, "\n");                                                 \
      __builtin_trap ();                                                      \
    }                                                                         \
  while (0)

/* **************************************************************** */
/*                             memmory                              */
/* **************************************************************** */

#define mem_malloc(size)                                                      \
  ({                                                                          \
    void *ret;                                                                \
    if (!(ret = malloc (size)))                                               \
      error ("can not allocate memory");                                      \
    ret;                                                                      \
  })

#define mem_realloc(ptr, size)                                                \
  ({                                                                          \
    void *ret;                                                                \
    if (!(ret = realloc (ptr, size)))                                         \
      error ("can not allocate memory");                                      \
    ret;                                                                      \
  })

#endif
