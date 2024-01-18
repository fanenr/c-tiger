#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stddef.h>

/* ********************************** */
/*             from util              */
/* ********************************** */

extern size_t chpos;
extern size_t linepos;

/* ********************************** */
/*             from lexer             */
/* ********************************** */

extern FILE *yyin;
extern int yyleng;
extern const char *yytext;

char *string (const char *src);
void *checked_malloc (size_t size);

void reset (const char *name);
void error (const char *fmt, ...);

#endif
