#include "mstr.h"

#include <stdlib.h>
#include <string.h>

#define set_len(str, new)                                                     \
  do                                                                          \
    {                                                                         \
      if (!mstr_is_sso (str))                                                 \
        (str)->heap.len = new;                                                \
      else                                                                    \
        (str)->sso.len = new;                                                 \
    }                                                                         \
  while (0)

void
mstr_free (mstr_t *str)
{
  if (mstr_is_heap (str))
    free (str->heap.data);
  *str = MSTR_INIT;
}

char *
mstr_release (mstr_t *str)
{
  if (mstr_is_sso (str))
    /* sso mstr can not be released */
    return NULL;

  char *data = str->heap.data;
  *str = MSTR_INIT;
  return data;
}

mstr_t *
mstr_reserve (mstr_t *str, size_t cap)
{
  char *newdata;
  size_t newcap;

  if ((newcap = mstr_cap (str)) >= cap)
    return str;

  /* compute capacity */
  while (newcap < cap)
    newcap *= MSTR_EXPAN_RATIO;

  if (newcap % 2)
    newcap++;

  if (mstr_is_heap (str))
    {
      newdata = realloc (str->heap.data, newcap);
      if (!newdata)
        return NULL;
    }
  else
    {
      if (!(newdata = malloc (newcap)))
        return NULL;

      /* copy to heap */
      size_t len = str->sso.len;
      if (memcpy (newdata, str->sso.data, len + 1) != newdata)
        { /* copy failed */
          free (newdata);
          return NULL;
        }

      /* save length */
      str->heap.len = len;
    }

  str->heap.data = newdata;
  str->heap.cap = newcap;
  return str;
}

mstr_t *
mstr_remove (mstr_t *str, size_t start, size_t n)
{
  if (!n)
    return str;

  size_t len = mstr_len (str);
  char *data = mstr_data (str);

  if (start >= len)
    /* out of range */
    return NULL;

  if (n > len - start)
    n = len - start;

  char *rmst = data + start;
  char *cpst = rmst + n;
  size_t cplen = len - start - n + 1;

  if (memmove (rmst, cpst, cplen) != rmst)
    /* move failed */
    return NULL;

  set_len (str, len - n);
  return str;
}

void
mstr_swap (mstr_t *a, mstr_t *b)
{
  mstr_t c = *a;
  *a = *b;
  *b = c;
}

void
mstr_move (mstr_t *new, mstr_t *old)
{
  mstr_free (new);
  *new = *old;
  *old = MSTR_INIT;
}

mstr_t *
mstr_substr (mstr_t *save, const mstr_t *from, size_t start, size_t n)
{
  size_t len = mstr_len (from);
  const char *pos = mstr_data (from) + start;

  if (start >= len)
    /* out of range */
    return NULL;

  if (n > len - start)
    n = len - start;

  return mstr_assign_byte (save, (const mstr_byte_t *)pos, n);
}

bool
mstr_start_with_char (const mstr_t *str, char ch)
{
  return mstr_start_with_byte (str, (const mstr_byte_t *)&ch, 1);
}

bool
mstr_start_with_cstr (const mstr_t *str, const char *cstr)
{
  return mstr_start_with_byte (str, (const mstr_byte_t *)cstr, strlen (cstr));
}

bool
mstr_start_with_mstr (const mstr_t *str, const mstr_t *other)
{
  return mstr_start_with_byte (str, (const mstr_byte_t *)mstr_data (other),
                               mstr_len (other));
}

bool
mstr_start_with_byte (const mstr_t *str, const mstr_byte_t *src, size_t n)
{
  if (n > mstr_len (str))
    return false;

  return memcmp (mstr_data (str), src, n) == 0;
}

bool
mstr_end_with_char (const mstr_t *str, char ch)
{
  return mstr_end_with_byte (str, (const mstr_byte_t *)&ch, 1);
}

bool
mstr_end_with_cstr (const mstr_t *str, const char *cstr)
{
  return mstr_end_with_byte (str, (const mstr_byte_t *)cstr, strlen (cstr));
}

bool
mstr_end_with_mstr (const mstr_t *str, const mstr_t *other)
{
  return mstr_end_with_byte (str, (const mstr_byte_t *)mstr_data (other),
                             mstr_len (other));
}

bool
mstr_end_with_byte (const mstr_t *str, const mstr_byte_t *src, size_t n)
{
  if (n > mstr_len (str))
    return false;

  const char *pos = mstr_data (str) + mstr_len (str) - n;
  return memcmp (pos, src, n) == 0;
}

int
mstr_cmp_cstr (const mstr_t *str, const char *cstr)
{
  return mstr_cmp_byte (str, (const mstr_byte_t *)cstr, strlen (cstr));
}

int
mstr_cmp_mstr (const mstr_t *str, const mstr_t *other)
{
  return mstr_cmp_byte (str, (const mstr_byte_t *)mstr_data (other),
                        mstr_len (other));
}

int
mstr_cmp_byte (const mstr_t *str, const mstr_byte_t *src, size_t n)
{
  size_t len = mstr_len (str);
  const char *data = mstr_data (str);
  int cmp_ret = memcmp (data, src, n > len ? len : n);

  if (cmp_ret != 0 || n == len)
    return cmp_ret;
  return len > n ? 1 : -1;
}

mstr_t *
mstr_cat_char (mstr_t *str, char ch)
{
  return mstr_cat_byte (str, (const mstr_byte_t *)&ch, 1);
}

mstr_t *
mstr_cat_cstr (mstr_t *str, const char *cstr)
{
  return mstr_cat_byte (str, (const mstr_byte_t *)cstr, strlen (cstr));
}

mstr_t *
mstr_cat_mstr (mstr_t *str, const mstr_t *other)
{
  return mstr_cat_byte (str, (const mstr_byte_t *)mstr_data (other),
                        mstr_len (other));
}

mstr_t *
mstr_cat_byte (mstr_t *str, const mstr_byte_t *src, size_t n)
{
  if (!n)
    return str;

  size_t len = mstr_len (str);
  if (mstr_reserve (str, len + n + 1) != str)
    /* allocate failed */
    return NULL;

  char *ctpos = mstr_data (str) + len;
  if (memcpy (ctpos, src, n) != ctpos)
    /* copy failed */
    return NULL;
  /* save NULL */
  ctpos[n] = '\0';

  set_len (str, len + n);
  return str;
}

mstr_t *
mstr_assign_char (mstr_t *str, char ch)
{
  return mstr_assign_byte (str, (const mstr_byte_t *)&ch, 1);
}

mstr_t *
mstr_assign_cstr (mstr_t *str, const char *cstr)
{
  return mstr_assign_byte (str, (const mstr_byte_t *)cstr, strlen (cstr));
}

mstr_t *
mstr_assign_mstr (mstr_t *str, const mstr_t *other)
{
  return mstr_assign_byte (str, (const mstr_byte_t *)mstr_data (other),
                           mstr_len (other));
}

mstr_t *
mstr_assign_byte (mstr_t *str, const mstr_byte_t *src, size_t n)
{
  if (!n)
    {
      mstr_data (str)[0] = '\0';
      set_len (str, 0);
      return str;
    }

  if (mstr_reserve (str, n + 1) != str)
    /* allocate failed */
    return NULL;

  char *data = mstr_data (str);
  if (memcpy (data, src, n) != data)
    /* copy failed */
    return NULL;
  data[n] = '\0';

  set_len (str, n);
  return str;
}
