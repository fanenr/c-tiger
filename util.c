#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *
checked_strdup (const char *src)
{
  size_t cap = strlen (src) + 1;
  char *ret = malloc (cap);
  if (ret == NULL)
    goto err;
  if (memcpy (ret, src, cap) != ret)
    goto err;
  return ret;
err:
  fprintf (stderr, "strdup failed\n");
  exit (1);
}

void *
checked_alloc (size_t size)
{
  void *ret = malloc (size);
  if (ret == NULL)
    goto err;
  if (memset (ret, 0, size) != ret)
    goto err;
  return ret;
err:
  fprintf (stderr, "malloc failed\n");
  exit (1);
}

void *
checked_realloc (void *ptr, size_t size)
{
  void *ret = realloc (ptr, size);
  if (ret == NULL)
    {
      fprintf (stderr, "realloc failed\n");
      exit (1);
    }
  return ret;
}
