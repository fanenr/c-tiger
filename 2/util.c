#include "util.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t chpos;
size_t linepos;
extern FILE *yyin;

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
  char *ret = checked_malloc (strlen (src) + 1);
  strcpy (ret, src);
  return ret;
}

void
reset (const char *name)
{
  chpos = linepos = 1;
  yyin = fopen (name, "r");
  if (yyin == NULL)
    {
      fprintf (stderr, "cannot open file: %s\n", name);
      exit (1);
    }
}

void
error (const char *fmt, ...)
{
  va_list ap;
  va_start (ap, fmt);
  fprintf (stderr, "error occured at %ld:%ld: ", linepos, chpos);
  vfprintf (stderr, fmt, ap);
  va_end (ap);
  exit (1);
}
