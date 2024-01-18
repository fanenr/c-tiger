#include "tiger.y.h"
#include "token.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

extern int yylex (void);
extern int yyparse (void);

/* token list */
static token *token_list;
static size_t token_size;
static size_t token_cap;

static void token_add (int type);

int
main (int argc, char **argv)
{
  if (argc != 2)
    {
      fprintf (stderr, "usage: %s *.tig\n", argv[0]);
      exit (1);
    }

  reset (argv[1]);

  yyparse ();
}

int
yywrap (void)
{
  return 1;
}

void
yyerror (const char *pos)
{
  fprintf (stderr, "error occured: %s\n", pos);
  exit (1);
}
