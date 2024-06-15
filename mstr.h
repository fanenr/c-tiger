#ifndef MSTR_H
#define MSTR_H

#include <stdbool.h>
#include <stddef.h>

#define MSTR_FLG_SSO 1
#define MSTR_FLG_HEAP 0

#define MSTR_EXPAN_RATIO 2
#define MSTR_SSO_CAP (sizeof (mstr_heap_t) - 1)

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
  char data[MSTR_SSO_CAP];
};

union mstr_t
{
  mstr_sso_t sso;
  mstr_heap_t heap;
};

#define MSTR_INIT                                                             \
  (mstr_t) { .sso.flg = MSTR_FLG_SSO }

#define mstr_is_sso(str) ((str)->sso.flg == MSTR_FLG_SSO)
#define mstr_is_heap(str) ((str)->sso.flg == MSTR_FLG_HEAP)

#define mstr_cap(str) (mstr_is_sso (str) ? MSTR_SSO_CAP : (str)->heap.cap)
#define mstr_len(str) (mstr_is_sso (str) ? (str)->sso.len : (str)->heap.len)
#define mstr_data(str) (mstr_is_sso (str) ? (str)->sso.data : (str)->heap.data)

#define mstr_at(str, pos) ((pos) < mstr_len (str) ? mstr_data (str)[pos] : -1)

/* free and release ownership */

extern void mstr_free (mstr_t *str) __attribute__ ((nonnull (1)));

extern char *mstr_release (mstr_t *str) __attribute__ ((nonnull (1)));

/* allocate space in advance */

extern mstr_t *mstr_reserve (mstr_t *str, size_t cap)
    __attribute__ ((nonnull (1)));

/* remove substring */

extern mstr_t *mstr_remove (mstr_t *str, size_t start, size_t n)
    __attribute__ ((nonnull (1)));

/* swap and move */

extern void mstr_swap (mstr_t *a, mstr_t *b) __attribute__ ((nonnull (1, 2)));

extern void mstr_move (mstr_t *new, mstr_t *old)
    __attribute__ ((nonnull (1, 2)));

/* substring */

extern mstr_t *mstr_substr (mstr_t *save, const mstr_t *from, size_t start,
                            size_t n) __attribute__ ((nonnull (1, 2)));

/* is start with */

extern bool mstr_start_with_char (const mstr_t *str, char ch)
    __attribute__ ((nonnull (1)));

extern bool mstr_start_with_cstr (const mstr_t *str, const char *cstr)
    __attribute__ ((nonnull (1, 2)));

extern bool mstr_start_with_mstr (const mstr_t *str, const mstr_t *other)
    __attribute__ ((nonnull (1, 2)));

extern bool mstr_start_with_byte (const mstr_t *str, const mstr_byte_t *src,
                                  size_t n) __attribute__ ((nonnull (1, 2)));

/* is end with */

extern bool mstr_end_with_char (const mstr_t *str, char ch)
    __attribute__ ((nonnull (1)));

extern bool mstr_end_with_cstr (const mstr_t *str, const char *cstr)
    __attribute__ ((nonnull (1, 2)));

extern bool mstr_end_with_mstr (const mstr_t *str, const mstr_t *other)
    __attribute__ ((nonnull (1, 2)));

extern bool mstr_end_with_byte (const mstr_t *str, const mstr_byte_t *src,
                                size_t n) __attribute__ ((nonnull (1, 2)));

/* compare */

extern int mstr_cmp_cstr (const mstr_t *str, const char *cstr)
    __attribute__ ((nonnull (1, 2)));

extern int mstr_cmp_mstr (const mstr_t *str, const mstr_t *other)
    __attribute__ ((nonnull (1, 2)));

extern int mstr_cmp_byte (const mstr_t *str, const mstr_byte_t *src, size_t n)
    __attribute__ ((nonnull (1, 2)));

/* concatenate */

extern mstr_t *mstr_cat_char (mstr_t *str, char ch)
    __attribute__ ((nonnull (1)));

extern mstr_t *mstr_cat_cstr (mstr_t *str, const char *cstr)
    __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_cat_mstr (mstr_t *str, const mstr_t *other)
    __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_cat_byte (mstr_t *str, const mstr_byte_t *src, size_t n)
    __attribute__ ((nonnull (1, 2)));

/* assign */

extern mstr_t *mstr_assign_char (mstr_t *str, char ch)
    __attribute__ ((nonnull (1)));

extern mstr_t *mstr_assign_cstr (mstr_t *str, const char *cstr)
    __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_assign_mstr (mstr_t *str, const mstr_t *other)
    __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_assign_byte (mstr_t *str, const mstr_byte_t *src, size_t n)
    __attribute__ ((nonnull (1, 2)));

#endif
