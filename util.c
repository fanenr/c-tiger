#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
quit (void)
{
  exit (1);
}

void *
checked_alloc (size_t size)
{
  void *ret;
  if (!(ret = calloc (1, size)))
    error ("can not allocate memory");
  return ret;
}

void *
checked_realloc (void *ptr, size_t size)
{
  void *ret;
  if (!(ret = realloc (ptr, size)))
    error ("can not allocate memory");
  return ret;
}

char *
checked_strdup (const char *src)
{
  char *ret;
  size_t cap = strlen (src) + 1;
  if (!(ret = malloc (cap)))
    error ("can not allocate memory");
  if (memcpy (ret, src, cap) != ret)
    error ("can not copy string\n");
  return ret;
}

long
checked_atol (const char *src)
{
  char *end;
  long ret = strtol (src, &end, 0);
  if (end == src)
    error ("invalid string\n");
  return ret;
}

double
checked_atod (const char *src)
{
  char *end;
  double ret = strtod (src, &end);
  if (end == src)
    error ("invalid string\n");
  return ret;
}

void
vector_init (vector *vec)
{
  vec->data = NULL;
  vec->size = vec->cap = 0;
}

bool
vector_reserve (vector *vec, size_t cap)
{
  if (vec->cap >= cap)
    return true;

  size_t ncap = vec->cap;
  if (ncap < VECTOR_INIT_CAP)
    ncap = VECTOR_INIT_CAP;

  while (ncap < cap)
    ncap *= VECTOR_EXPAN_RATIO;

  void **ndat = realloc (vec->data, ncap * sizeof (void *));
  if (ndat == NULL)
    return false;

  vec->data = ndat;
  vec->cap = ncap;
  return true;
}

void
vector_free (vector *vec)
{
  free (vec->data);
  vector_init (vec);
  return;
}

void *
vector_get (vector *vec, size_t pos)
{
  if (pos >= vec->size)
    return NULL;
  return vec->data[pos];
}

bool
vector_insert (vector *vec, size_t pos, void *node)
{
  if (pos > vec->size)
    return false;

  if (!vector_reserve (vec, vec->size + 1))
    return false;

  if (pos == vec->size)
    return vector_push_back (vec, node);

  void **data = vec->data;
  void **mvde = data + pos + 1;
  size_t mvlen = (vec->size - pos) * sizeof (void *);
  if (memmove (mvde, mvde - 1, mvlen) != mvde)
    return false;

  vec->size++;
  data[pos] = node;
  return true;
}

bool
vector_push_back (vector *vec, void *node)
{
  if (!vector_reserve (vec, vec->size + 1))
    return false;

  vec->data[vec->size++] = node;
  return true;
}

bool
vector_erase (vector *vec, size_t pos)
{
  if (pos >= vec->size)
    return false;

  if (pos == vec->size - 1)
    {
      vec->size--;
      return true;
    }

  void **data = vec->data;
  void **mvde = data + pos;
  size_t mvlen = (vec->size - pos - 1) * sizeof (void *);
  if (memmove (mvde, mvde + 1, mvlen) != mvde)
    return false;

  vec->size--;
  return true;
}

void
stack_init (stack *stac)
{
  vector_init (&stac->vec);
}

bool
stack_reserve (stack *stac, size_t cap)
{
  return vector_reserve (&stac->vec, cap);
}

void
stack_free (stack *stac)
{
  vector_free (&stac->vec);
  stack_init (stac);
}

bool
stack_push (stack *stac, void *node)
{
  return vector_push_back (&stac->vec, node);
}

void *
stack_top (stack *stac)
{
  return vector_get (&stac->vec, stac->vec.size - 1);
}

bool
stack_pop (stack *stac)
{
  return vector_erase (&stac->vec, stac->vec.size - 1);
}
