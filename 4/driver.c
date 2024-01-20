#include "lexer.h"
#include "parser.h"
#include "tiger.y.h"
#include <stdio.h>
#include <stdlib.h>

extern FILE *yyin;
extern int yylex (void);
extern int yyparse (void);

static void token_add (int type);

int
main (int argc, char **argv)
{
  if (argc != 2)
    {
      fprintf (stderr, "usage: %s *.tig\n", argv[0]);
      exit (1);
    }

  yyin = fopen (argv[1], "r");
  if (yyin == NULL)
    {
      fprintf (stderr, "open file %s failed\n", argv[1]);
      exit (1);
    }

  yyparse ();

  for (unsigned i = 0; i < stms.size; i++)
    {
      ast_stm *stm = stms.list[i];
      printf ("stm `%u`, kind `%u`, at `%u:%u`\n", i + 1, stm->kind,
              stm->lnpos, stm->chpos);
    }
}

int
yywrap (void)
{
  return 1;
}

void
yyerror (const char *pos)
{
  error ("%s", pos);
}