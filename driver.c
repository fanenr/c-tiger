#include "ast.h"
#include "parser.h"
#include "tiger.y.h"
#include <stdio.h>
#include <stdlib.h>

extern FILE *yyin;
extern int yylex (void);
extern int yyparse (void);

extern ast_env *m_env;

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

  ast_env_init ();

  yyparse ();
}
