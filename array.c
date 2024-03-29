#include "array_ext.h"

void *
array_insert (array_t *arr, size_t pos)
{
  size_t size = arr->size;

  if (gcc_unlikely (pos == size))
    return array_push_back (arr);

  if (gcc_unlikely (pos > size || size >= arr->cap))
    return NULL;

  size_t elem_size = arr->elem_size;
  void *in = arr->data + pos * elem_size;
  size_t len = (size - pos) * elem_size;
  void *next = in + elem_size;

  if (gcc_memmove (next, in, len) != next)
    return NULL;

  arr->size++;
  return in;
}

void
array_erase (array_t *arr, size_t pos)
{
  size_t size = arr->size;

  if (gcc_unlikely (pos >= arr->size))
    return;

  void *rm = array_at (arr, pos);
  size_t elem_size = arr->elem_size;

  if (pos == size - 1)
    goto dec_size;

  void *next = rm + elem_size;
  size_t len = (size - pos - 1) * elem_size;

  if (gcc_memmove (rm, next, len) != rm)
    return;

dec_size:
  arr->size--;
}

void *
array_push_back (array_t *arr)
{
  if (gcc_unlikely (arr->size >= arr->cap))
    return NULL;
  return arr->data + arr->elem_size * arr->size++;
}

/* **************************************************************** */
/*                               ext                                */
/* **************************************************************** */

void *
array_find (const array_t *arr, const void *target, array_comp_t *comp)
{
  size_t elem_size = arr->elem_size;

  void *curr = arr->data;
  for (size_t size = arr->size; size; size--)
    {
      if (comp (target, curr) == 0)
        return curr;
      curr += elem_size;
    }

  return NULL;
}

void
array_for_each (array_t *arr, array_visit_t *visit)
{
  void *data = arr->data;
  size_t elem_size = arr->elem_size;

  void *elem = data - elem_size;
  for (size_t size = arr->size; size; size--)
    visit ((elem += elem_size));
}
