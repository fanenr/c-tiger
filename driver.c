#include "ast.h"
#include "parser.h"
#include "tiger.y.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

extern FILE *yyin;
extern int yylex (void);
extern int yyparse (void);

extern ast_env *m_env;

void print_wsp (int n);
void print (ast_env *env, int l);

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

  printf ("prog\n");
  print (&prog, 1);
}

void
print (ast_env *env, int l)
{
  if (!env)
    return;

  vector *defs = &env->defs;
  print_wsp (l);
  printf ("defs: %lu\n", defs->size);
  for (size_t i = 0; i < defs->size; i++)
    {
      ast_def *def = defs->data[i];
      if (def->kind == AST_DEF_TYPE)
        {
          ast_def_type *get = AST_DEF_GET (type, def);
          ast_type *type = get->type;
          if (type->kind == AST_TYPE_UNION)
            {
              print_wsp (l);
              printf ("union %s\n", def->id.str);
              print (type->mem, l + 1);
            }
          else if (type->kind == AST_TYPE_STRUCT)
            {
              print_wsp (l);
              printf ("struct %s\n", def->id.str);
              print (type->mem, l + 1);
            }
          else
            {
              print_wsp (l);
              printf ("type %s\n", def->id.str);
            }
        }

      if (def->kind == AST_DEF_VAR)
        {
          print_wsp (l);
          printf ("var %s\n", def->id.str);
        }

      if (def->kind == AST_DEF_FUNC)
        {
          ast_def_func *get = AST_DEF_GET (func, def);
          print_wsp (l);
          printf ("func %s\n", def->id.str);
          print (get->env, l + 1);
        }
    }

  vector *stms = &env->stms;
  print_wsp (l);
  printf ("stms: %lu\n", stms->size);
  for (size_t i = 0; i < stms->size; i++)
    {
      ast_stm *stm = stms->data[i];
      if (stm->kind == AST_STM_ASSIGN)
        {
          ast_stm_assign *get = AST_STM_GET (assign, stm);
          print_wsp (l);
          printf ("assign\n");
        }

      if (stm->kind == AST_STM_WHILE)
        {
          ast_stm_while *get = AST_STM_GET (while, stm);
          print_wsp (l);
          printf ("while\n");
          print (get->env, l + 1);
        }

      if (stm->kind == AST_STM_RETURN)
        {
          ast_stm_return *get = AST_STM_GET (return, stm);
          print_wsp (l);
          printf ("return\n");
        }

      if (stm->kind == AST_STM_IF1)
        {
          ast_stm_if *get = AST_STM_GET (if, stm);
          print_wsp (l);
          printf ("if\n");
          print (get->then_env, l + 1);
        }

      if (stm->kind == AST_STM_IF2)
        {
          ast_stm_if *get = AST_STM_GET (if, stm);
          print_wsp (l);
          printf ("if\n");
          print (get->then_env, l + 1);
          print (get->else_env, l + 1);
        }
    }
}

void
print_wsp (int n)
{
  for (int i = 0; i < 4 * n; i++)
    printf (" ");
}
