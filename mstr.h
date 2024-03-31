#ifndef MSTR_H
#define MSTR_H

#include <stdbool.h>
#include <stddef.h>

#define MSTR_EXPAN_RATIO 2
#define MSTR_INIT_CAP (sizeof (mstr_t))
#define MSTR_SSO_MAXCAP (sizeof (mstr_t) - 1)

typedef union mstr_t mstr_t;
typedef unsigned char mstr_byte_t;
typedef struct mstr_sso_t mstr_sso_t;
typedef struct mstr_heap_t mstr_heap_t;

struct mstr_heap_t
{
  size_t cap;
  size_t len;
  char *data;
};

struct mstr_sso_t
{
  mstr_byte_t flg : 1;
  mstr_byte_t len : 7;
  char data[sizeof (mstr_heap_t) - 1];
};

union mstr_t
{
  mstr_sso_t sso;
  mstr_heap_t heap;
};

#define MSTR_INIT                                                             \
  (mstr_t) (mstr_sso_t) { .flg = true }

extern void mstr_init (mstr_t *str) __attribute__ ((nonnull (1)));

extern void mstr_free (mstr_t *str) __attribute__ ((nonnull (1)));

extern size_t mstr_cap (const mstr_t *str) __attribute__ ((nonnull (1)));

extern size_t mstr_len (const mstr_t *str) __attribute__ ((nonnull (1)));

extern const char *mstr_data (const mstr_t *str) __attribute__ ((nonnull (1)));

extern int mstr_at (const mstr_t *str, size_t pos)
    __attribute__ ((nonnull (1)));

extern char *mstr_release (mstr_t *str) __attribute__ ((nonnull (1)));

extern mstr_t *mstr_reserve (mstr_t *dest, size_t cap)
    __attribute__ ((nonnull (1)));

extern mstr_t *mstr_remove (mstr_t *dest, size_t spos, size_t slen)
    __attribute__ ((nonnull (1)));

extern void mstr_swap (mstr_t *dest, mstr_t *src)
    __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_move (mstr_t *dest, mstr_t *src)
    __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_substr (mstr_t *dest, const mstr_t *src, size_t spos,
                            size_t slen) __attribute__ ((nonnull (1, 2)));

extern bool mstr_start_with_char (const mstr_t *str, char src)
    __attribute__ ((nonnull (1)));

extern bool mstr_start_with_cstr (const mstr_t *str, const char *src)
    __attribute__ ((nonnull (1, 2)));

extern bool mstr_start_with_mstr (const mstr_t *str, const mstr_t *src)
    __attribute__ ((nonnull (1)));

extern bool mstr_start_with_byte (const mstr_t *str, const mstr_byte_t *src,
                                  size_t slen) __attribute__ ((nonnull (1)));

extern bool mstr_end_with_char (const mstr_t *str, char src)
    __attribute__ ((nonnull (1)));

extern bool mstr_end_with_cstr (const mstr_t *str, const char *src)
    __attribute__ ((nonnull (1, 2)));

extern bool mstr_end_with_mstr (const mstr_t *str, const mstr_t *src)
    __attribute__ ((nonnull (1)));

extern bool mstr_end_with_byte (const mstr_t *str, const mstr_byte_t *src,
                                size_t slen) __attribute__ ((nonnull (1)));

extern int mstr_cmp_cstr (const mstr_t *str, const char *src)
    __attribute__ ((nonnull (1, 2)));

extern int mstr_cmp_mstr (const mstr_t *str, const mstr_t *src)
    __attribute__ ((nonnull (1, 2)));

extern int mstr_cmp_byte (const mstr_t *str, const mstr_byte_t *src,
                          size_t slen) __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_cat_char (mstr_t *dest, char src)
    __attribute__ ((nonnull (1)));

extern mstr_t *mstr_cat_cstr (mstr_t *dest, const char *src)
    __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_cat_mstr (mstr_t *dest, const mstr_t *src)
    __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_cat_byte (mstr_t *dest, const mstr_byte_t *src,
                              size_t slen) __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_assign_char (mstr_t *dest, char src)
    __attribute__ ((nonnull (1)));

extern mstr_t *mstr_assign_cstr (mstr_t *dest, const char *src)
    __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_assign_mstr (mstr_t *dest, const mstr_t *src)
    __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_assign_byte (mstr_t *dest, const mstr_byte_t *src,
                                 size_t slen) __attribute__ ((nonnull (1, 2)));

#endif
