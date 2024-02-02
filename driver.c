#include "ast.h"
#include "parser.h"
#include "sema.h"
#include "tiger.y.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

extern FILE *yyin;
extern int yylex (void);
extern int yyparse (void);

extern ast_env *m_env;

void print_wsp (int n);
void print_type (ast_type *type);
void print (ast_env *env, int l);

int
main (int argc, char **argv)
{
  if (argc < 2)
    error ("usage: %s *.tig\n", argv[0]);

  yyin = fopen (argv[1], "r");
  if (yyin == NULL)
    error ("open file %s failed\n", argv[1]);

  ast_env_init ();
  yyparse ();
  sema_check (&prog);

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
              printf ("type %s: ", def->id.str);
              print_type (type);
              printf ("\n");
            }
        }

      if (def->kind == AST_DEF_VAR)
        {
          print_wsp (l);
          printf ("var %s: ", def->id.str);
          print_type (AST_DEF_GET (var, def)->type);
          printf ("\n");
        }

      if (def->kind == AST_DEF_FUNC)
        {
          ast_def_func *get = AST_DEF_GET (func, def);
          print_wsp (l);
          printf ("func %s: ", def->id.str);
          print_type (get->type);
          printf ("\n");
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
print_type (ast_type *type)
{
  switch (type->kind)
    {
    case AST_TYPE_BASE_ST + 1 ... AST_TYPE_BASE_ED - 1:
      {
        printf ("%s", base_type_name[type->kind]);
        break;
      }
    case AST_TYPE_POINTER:
      {
        printf ("*");
        print_type (type->ref);
        break;
      }
    }
}

void
print_wsp (int n)
{
  for (int i = 0; i < n; i++)
    printf ("    ");
}
