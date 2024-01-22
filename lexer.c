#include "ast.h"
#include "lexer.h"
#include "tiger.y.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/* util */
char *string (const char *src);
void *checked_malloc (size_t size);
void *checked_realloc (void *ptr, size_t size);

/* extern */
extern int yyleng;
extern const char *yytext;

/* global */
ast_pos pos = { .ln = 1, .ch = 0 };

void
adjust (void)
{
  pos.ch += yyleng;
}

void
nline (void)
{
  pos.ln++;
  pos.ch = 0;
}

void
other (void)
{
  error ("unknown type token %s\n", yytext);
}

int
handle (int tok)
{
  switch (tok)
    {
    case ID:
    case STR:
      yylval.ptr = string (yytext);
      break;

    case NUM:
      yylval.num = atoi (yytext);
      break;

    case REAL:
      yylval.real = atof (yytext);
      break;

    default:
      yylval.pos = pos;
      break;
    }
  return tok;
}

void
error (const char *fmt, ...)
{
  va_list ap;
  va_start (ap, fmt);
  fprintf (stderr, "error occured at %u:%u: ", pos.ln, pos.ch);
  vfprintf (stderr, fmt, ap);
  va_end (ap);
  exit (1);
}

/* ************************************************ */
/*                     util                         */
/* ************************************************ */

void *
checked_malloc (size_t size)
{
  void *ret = malloc (size);
  if (ret == NULL)
    {
      fprintf (stderr, "malloc failed\n");
      exit (1);
    }
  return ret;
}

char *
string (const char *src)
{
  size_t cap = strlen (src) + 1;
  char *ret = checked_malloc (cap);
  if (memcpy (ret, src, cap) != ret)
    {
      fprintf (stderr, "string failed\n");
      exit (1);
    }
  return ret;
}

void *
checked_realloc (void *ptr, size_t size)
{
  void *ret = realloc (ptr, size);
  if (ret == NULL)
    {
      fprintf (stderr, "realloc failed\n");
      exit (1);
    }
  return ret;
}
