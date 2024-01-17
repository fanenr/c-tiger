#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <stddef.h>

extern size_t chpos;
extern size_t linepos;

extern int yyleng;
extern const char *yytext;

char *string (const char *src);
void *checked_malloc (size_t size);

void error (const char *fmt, ...);
void reset (const char *name);

#endif
