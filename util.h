#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

extern void *checked_alloc (size_t size);
extern char *checked_strdup (const char *src);
extern void *checked_realloc (void *ptr, size_t size);

#endif