#include "mstr.h"
#include <string.h>

#ifndef MSTR_MEM
#include <stdlib.h>
#define mstr_mem_free free
#define mstr_mem_malloc malloc
#define mstr_mem_realloc realloc
#else
#include "mstr_mem.h"
#endif

#define is_sso(STR) ((STR)->sso.flg)

#define get_len(STR) (!is_sso (STR) ? (STR)->heap.len : (STR)->sso.len)
#define get_cap(STR) (!is_sso (STR) ? (STR)->heap.cap : MSTR_SSO_MAXCAP)
#define get_data(STR) (!is_sso (STR) ? (STR)->heap.data : (STR)->sso.data)

#define set_len(STR, LEN)                                                     \
  do                                                                          \
    {                                                                         \
      if (!is_sso (STR))                                                      \
        (STR)->heap.len = (LEN);                                              \
      else                                                                    \
        (STR)->sso.len = (LEN);                                               \
    }                                                                         \
  while (0)

void
mstr_init (mstr_t *str)
{
  *str = (mstr_t)(mstr_sso_t){ .flg = true };
}

void
mstr_free (mstr_t *str)
{
  if (!is_sso (str))
    mstr_mem_free (str->heap.data);
  mstr_init (str);
}

size_t
mstr_cap (const mstr_t *str)
{
  return get_cap (str);
}

size_t
mstr_len (const mstr_t *str)
{
  return get_len (str);
}

const char *
mstr_data (const mstr_t *str)
{
  return get_data (str);
}

int
mstr_at (const mstr_t *str, size_t pos)
{
  if (pos >= get_len (str))
    return -1;
  return get_data (str)[pos];
}

char *
mstr_release (mstr_t *str)
{
  if (is_sso (str))
    /* sso mstr can not be released */
    return NULL;

  char *data = str->heap.data;
  mstr_init (str);
  return data;
}

mstr_t *
mstr_reserve (mstr_t *dest, size_t cap)
{
  bool flg = is_sso (dest);
  if (flg && cap <= MSTR_SSO_MAXCAP)
    return dest;
  if (!flg && cap <= dest->heap.cap)
    return dest;

  size_t ncap = MSTR_INIT_CAP;
  if (!flg && dest->heap.cap > ncap)
    ncap = dest->heap.cap;

  /* compute the capacity to allocate */
  while (ncap < cap)
    ncap *= MSTR_EXPAN_RATIO;

  char *data = flg ? mstr_mem_malloc (ncap)
                   : mstr_mem_realloc (dest->heap.data, ncap);
  if (data == NULL)
    /* malloc failed */
    return NULL;

  if (flg)
    {
      /* copy sso mstr into heap */
      if (memcpy (data, dest->sso.data, dest->sso.len + 1) != data)
        { /* copy failed */
          mstr_mem_free (data);
          return NULL;
        }
      /* save the length of sso mstr */
      dest->heap.len = dest->sso.len;
    }

  dest->heap.data = data;
  dest->heap.cap = ncap;
  return dest;
}

mstr_t *
mstr_remove (mstr_t *dest, size_t spos, size_t slen)
{
  if (!slen)
    return dest;

  size_t len = get_len (dest);
  if (spos >= len)
    /* out of range */
    return NULL;

  if (slen > len - spos)
    slen = len - spos;

  char *data = get_data (dest);
  char *rmst = data + spos;
  char *cpst = rmst + slen;
  size_t cplen = len - spos - slen + 1;
  if (memmove (rmst, cpst, cplen) != rmst)
    /* move failed */
    return NULL;

  set_len (dest, len - slen);
  return dest;
}

void
mstr_swap (mstr_t *dest, mstr_t *src)
{
  mstr_t copy = *dest;
  *dest = *src;
  *src = copy;
}

mstr_t *
mstr_move (mstr_t *dest, mstr_t *src)
{
  mstr_free (dest);
  *dest = *src;
  mstr_init (src);
  return dest;
}

mstr_t *
mstr_substr (mstr_t *dest, const mstr_t *src, size_t spos, size_t slen)
{
  size_t len = get_len (src);
  if (spos >= len)
    /* out of range */
    return NULL;

  if (slen > len - spos)
    slen = len - spos;

  const char *pos = get_data (src) + spos;
  return mstr_assign_byte (dest, (const mstr_byte_t *)pos, slen);
}

bool
mstr_start_with_char (const mstr_t *str, char src)
{
  return mstr_start_with_byte (str, (const mstr_byte_t *)&src, 1);
}

bool
mstr_start_with_cstr (const mstr_t *str, const char *src)
{
  return mstr_start_with_byte (str, (const mstr_byte_t *)src, strlen (src));
}

bool
mstr_start_with_mstr (const mstr_t *str, const mstr_t *src)
{
  return mstr_start_with_byte (str, (const mstr_byte_t *)get_data (src),
                               get_len (src));
}

bool
mstr_start_with_byte (const mstr_t *str, const mstr_byte_t *src, size_t slen)
{
  if (slen > get_len (str))
    return false;

  return memcmp (get_data (str), src, slen);
}

bool
mstr_end_with_char (const mstr_t *str, char src)
{
  return mstr_end_with_byte (str, (const mstr_byte_t *)&src, 1);
}

bool
mstr_end_with_cstr (const mstr_t *str, const char *src)
{
  return mstr_end_with_byte (str, (const mstr_byte_t *)src, strlen (src));
}

bool
mstr_end_with_mstr (const mstr_t *str, const mstr_t *src)
{
  return mstr_end_with_byte (str, (const mstr_byte_t *)get_data (src),
                             get_len (src));
}

bool
mstr_end_with_byte (const mstr_t *str, const mstr_byte_t *src, size_t slen)
{
  if (slen > get_len (str))
    return false;

  const char *data = get_data (str) + get_len (str) - slen;
  return memcmp (data, src, slen);
}

int
mstr_cmp_cstr (const mstr_t *str, const char *src)
{
  return mstr_cmp_byte (str, (const mstr_byte_t *)src, strlen (src));
}

int
mstr_cmp_mstr (const mstr_t *str, const mstr_t *src)
{
  return mstr_cmp_byte (str, (const mstr_byte_t *)get_data (src),
                        get_len (src));
}

int
mstr_cmp_byte (const mstr_t *str, const mstr_byte_t *src, size_t slen)
{
  size_t len = get_len (str);
  const char *data = get_data (str);
  int cmp = memcmp (data, src, slen > len ? len : slen);

  if (cmp != 0 || slen == len)
    return cmp;
  return len > slen ? 1 : -1;
}

mstr_t *
mstr_cat_char (mstr_t *dest, char src)
{
  return mstr_cat_byte (dest, (const mstr_byte_t *)&src, 1);
}

mstr_t *
mstr_cat_cstr (mstr_t *dest, const char *src)
{
  return mstr_cat_byte (dest, (const mstr_byte_t *)src, strlen (src));
}

mstr_t *
mstr_cat_mstr (mstr_t *dest, const mstr_t *src)
{
  return mstr_cat_byte (dest, (const mstr_byte_t *)get_data (src),
                        get_len (src));
}

mstr_t *
mstr_cat_byte (mstr_t *dest, const mstr_byte_t *src, size_t slen)
{
  if (!slen)
    return dest;

  size_t len = get_len (dest);
  if (mstr_reserve (dest, len + slen + 1) != dest)
    /* allocate failed */
    return NULL;

  char *ctpos = get_data (dest) + len;
  if (memcpy (ctpos, src, slen) != ctpos)
    /* copy failed */
    return NULL;
  /* save NULL */
  ctpos[slen] = '\0';

  set_len (dest, len + slen);
  return dest;
}

mstr_t *
mstr_assign_char (mstr_t *dest, char src)
{
  return mstr_assign_byte (dest, (const mstr_byte_t *)&src, 1);
}

mstr_t *
mstr_assign_cstr (mstr_t *dest, const char *src)
{
  return mstr_assign_byte (dest, (const mstr_byte_t *)src, strlen (src));
}

mstr_t *
mstr_assign_mstr (mstr_t *dest, const mstr_t *src)
{
  return mstr_assign_byte (dest, (const mstr_byte_t *)get_data (src),
                           get_len (src));
}

mstr_t *
mstr_assign_byte (mstr_t *dest, const mstr_byte_t *src, size_t slen)
{
  if (!slen)
    {
      get_data (dest)[0] = '\0';
      set_len (dest, 0);
      return dest;
    }

  if (mstr_reserve (dest, slen + 1) != dest)
    /* allocate failed */
    return NULL;

  char *data = get_data (dest);
  if (memcpy (data, src, slen) != data)
    /* copy failed */
    return NULL;
  data[slen] = '\0';

  set_len (dest, slen);
  return dest;
}
