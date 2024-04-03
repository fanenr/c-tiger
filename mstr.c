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

#define is_sso(STR) ((STR)->sso.flg == MSTR_FLG_SSO)
#define is_heap(STR) ((STR)->sso.flg == MSTR_FLG_HEAP)

#define cap_of(STR) (is_heap (STR) ? (STR)->heap.cap : MSTR_SSO_CAP)
#define len_of(STR) (is_heap (STR) ? (STR)->heap.len : (STR)->sso.len)
#define data_of(STR) (is_heap (STR) ? (STR)->heap.data : (STR)->sso.data)

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
  *str = (mstr_t)(mstr_sso_t){ .flg = MSTR_FLG_SSO };
}

void
mstr_free (mstr_t *str)
{
  if (is_heap (str))
    mstr_mem_free (str->heap.data);
  mstr_init (str);
}

size_t
mstr_cap (const mstr_t *str)
{
  return cap_of (str);
}

size_t
mstr_len (const mstr_t *str)
{
  return len_of (str);
}

const char *
mstr_data (const mstr_t *str)
{
  return data_of (str);
}

int
mstr_at (const mstr_t *str, size_t pos)
{
  if (pos >= len_of (str))
    return -1;
  return data_of (str)[pos];
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
  char *newdata;
  size_t newcap;

  if ((newcap = cap_of (dest)) >= cap)
    return dest;

  /* compute capacity */
  if (newcap == MSTR_SSO_CAP)
    newcap++;

  while (newcap < cap)
    newcap *= MSTR_EXPAN_RATIO;

  if (is_heap (dest))
    {
      newdata = mstr_mem_realloc (dest->heap.data, newcap);
      if (!newdata)
        return NULL;
    }
  else
    {
      if (!(newdata = mstr_mem_malloc (newcap)))
        return NULL;

      /* copy to heap */
      size_t len = dest->sso.len;
      if (memcpy (newdata, dest->sso.data, len + 1) != newdata)
        { /* copy failed */
          mstr_mem_free (newdata);
          return NULL;
        }

      /* save length */
      dest->heap.len = len;
    }

  dest->heap.data = newdata;
  dest->heap.cap = newcap;
  return dest;
}

mstr_t *
mstr_remove (mstr_t *dest, size_t spos, size_t slen)
{
  if (!slen)
    return dest;

  size_t len = len_of (dest);
  char *data = data_of (dest);

  if (spos >= len)
    /* out of range */
    return NULL;

  if (slen > len - spos)
    slen = len - spos;

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
  size_t len = len_of (src);
  const char *pos = data_of (src) + spos;

  if (spos >= len)
    /* out of range */
    return NULL;

  if (slen > len - spos)
    slen = len - spos;

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
  return mstr_start_with_byte (str, (const mstr_byte_t *)data_of (src),
                               len_of (src));
}

bool
mstr_start_with_byte (const mstr_t *str, const mstr_byte_t *src, size_t slen)
{
  if (slen > len_of (str))
    return false;

  return memcmp (data_of (str), src, slen) == 0;
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
  return mstr_end_with_byte (str, (const mstr_byte_t *)data_of (src),
                             len_of (src));
}

bool
mstr_end_with_byte (const mstr_t *str, const mstr_byte_t *src, size_t slen)
{
  if (slen > len_of (str))
    return false;

  const char *pos = data_of (str) + len_of (str) - slen;
  return memcmp (pos, src, slen) == 0;
}

int
mstr_cmp_cstr (const mstr_t *str, const char *src)
{
  return mstr_cmp_byte (str, (const mstr_byte_t *)src, strlen (src));
}

int
mstr_cmp_mstr (const mstr_t *str, const mstr_t *src)
{
  return mstr_cmp_byte (str, (const mstr_byte_t *)data_of (src), len_of (src));
}

int
mstr_cmp_byte (const mstr_t *str, const mstr_byte_t *src, size_t slen)
{
  size_t len = len_of (str);
  const char *data = data_of (str);
  int cmp_ret = memcmp (data, src, slen > len ? len : slen);

  if (cmp_ret != 0 || slen == len)
    return cmp_ret;
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
  return mstr_cat_byte (dest, (const mstr_byte_t *)data_of (src),
                        len_of (src));
}

mstr_t *
mstr_cat_byte (mstr_t *dest, const mstr_byte_t *src, size_t slen)
{
  if (!slen)
    return dest;

  size_t len = len_of (dest);
  if (mstr_reserve (dest, len + slen + 1) != dest)
    /* allocate failed */
    return NULL;

  char *ctpos = data_of (dest) + len;
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
  return mstr_assign_byte (dest, (const mstr_byte_t *)data_of (src),
                           len_of (src));
}

mstr_t *
mstr_assign_byte (mstr_t *dest, const mstr_byte_t *src, size_t slen)
{
  if (!slen)
    {
      data_of (dest)[0] = '\0';
      set_len (dest, 0);
      return dest;
    }

  if (mstr_reserve (dest, slen + 1) != dest)
    /* allocate failed */
    return NULL;

  char *data = data_of (dest);
  if (memcpy (data, src, slen) != data)
    /* copy failed */
    return NULL;
  data[slen] = '\0';

  set_len (dest, slen);
  return dest;
}
