#include "array.h"
#include "ast.h"
#include "common.h"
#include "mstr.h"
#include "parser.h"
#include "tiger.y.h"

#include <stdio.h>
#include <stdlib.h>

extern FILE *yyin;
extern int yylex (void);
extern int yyparse (void);

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

  ast_prog_init ();

  yyparse ();

  printf ("prog\n");
  print (&prog, 1);
}

void
print (ast_env *env, int l)
{
  if (!env)
    return;

  array_t *defs = &env->defs;
  array_t *stms = &env->stms;

  print_wsp (l);

  printf ("defs: %lu\n", defs->size);
  for (size_t i = 0; i < defs->size; i++)
    {
      ast_def *base = *(ast_def **) array_at (defs, i);

      if (base->kind == AST_DEF_TYPE)
	{
	  ast_def_type *def = container_of (base, ast_def_type, base);
	  ast_type *type = def->type;

	  if (type->kind == AST_TYPE_UNION)
	    {
	      print_wsp (l);
	      printf ("union %s\n", mstr_data (&base->name));
	      print (type->mem, l + 1);
	    }
	  else if (type->kind == AST_TYPE_STRUCT)
	    {
	      print_wsp (l);
	      printf ("struct %s\n", mstr_data (&base->name));
	      print (type->mem, l + 1);
	    }
	  else
	    {
	      print_wsp (l);
	      printf ("type %s: ", mstr_data (&base->name));
	      print_type (type);
	      printf ("\n");
	    }
	}

      if (base->kind == AST_DEF_VAR)
	{
	  print_wsp (l);
	  printf ("var %s: ", mstr_data (&base->name));
	  ast_def_var *def = container_of (base, ast_def_var, base);
	  print_type (def->type);
	  printf ("\n");
	}

      if (base->kind == AST_DEF_FUNC)
	{
	  print_wsp (l);
	  printf ("func %s: ", mstr_data (&base->name));
	  ast_def_func *def = container_of (base, ast_def_func, base);
	  print_type (def->type);
	  printf ("\n");
	  print (def->env, l + 1);
	}
    }

  print_wsp (l);

  printf ("stms: %lu\n", stms->size);
  for (size_t i = 0; i < stms->size; i++)
    {
      ast_stm *base = *(ast_stm **) array_at (stms, i);

      if (base->kind == AST_STM_ASSIGN)
	{
	  print_wsp (l);
	  printf ("assign\n");
	}

      if (base->kind == AST_STM_WHILE)
	{
	  ast_stm_while *stm = container_of (base, ast_stm_while, base);
	  print_wsp (l);
	  printf ("while\n");
	  print (stm->env, l + 1);
	}

      if (base->kind == AST_STM_RETURN)
	{
	  print_wsp (l);
	  printf ("return\n");
	}

      if (base->kind == AST_STM_IF)
	{
	  ast_stm_if *stm = container_of (base, ast_stm_if, base);
	  print_wsp (l);
	  printf ("if\n");
	  print (stm->then_env, l + 1);
	  print (stm->else_env, l + 1);
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
    case AST_TYPE_UNION:
      {
	printf ("union");
	break;
      }
    case AST_TYPE_STRUCT:
      {
	printf ("struct");
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
