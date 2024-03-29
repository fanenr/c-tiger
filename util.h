#ifndef UTIL_H
#define UTIL_H

#define gcc_likely(exp) __builtin_expect (!!(exp), 1)

#define gcc_unlikely(exp) __builtin_expect (!!(exp), 0)

#define gcc_memcpy(dest, src, n) __builtin_memcpy ((dest), (src), (n))

#define gcc_memmove(dest, src, n) __builtin_memmove ((dest), (src), (n))

#define gcc_same_type(a, b)                                                   \
  __builtin_types_compatible_p (typeof (a), typeof (b))

#define attr_nonnull(...) __attribute__ ((nonnull (__VA_ARGS__)))

#define container_of(ptr, type, member)                                       \
  ((type *)((void *)(ptr)-offsetof (type, member)))

#endif
