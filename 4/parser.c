#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

extern char *string (const char *str);
extern void *checked_malloc (size_t size);

ast_stm *
ast_stm_new (int type, ast_pos pos, ...)
{
  static unsigned sizes[]
      = { [AST_STM_ASSIGN] = AST_STM_SIZE (assign),
          [AST_STM_WHILE] = AST_STM_SIZE (while),
          [(AST_STM_IF_ST + 1)...(AST_STM_IF_ED - 1)] = AST_STM_SIZE (if),
        };

  ast_stm *ret = checked_malloc (sizes[type]);
  ret->kind = type;
  ret->pos = pos;

  va_list ap;
  va_start (ap, pos);

  switch (type)
    {
    case AST_STM_ASSIGN:
      {
        ast_stm_assign *get = AST_STM_GET (assign, ret);
        get->exp1 = va_arg (ap, ast_exp *);
        get->exp2 = va_arg (ap, ast_exp *);
        break;
      }
    case AST_STM_WHILE:
      {
        ast_stm_while *get = AST_STM_GET (while, ret);
        get->exp = va_arg (ap, ast_exp *);
        get->stms = va_arg (ap, ast_stms *);
        break;
      }
    case AST_STM_IF1:
      {
        ast_stm_if *get = AST_STM_GET (if, ret);
        get->exp = va_arg (ap, ast_exp *);
        get->then_stms = va_arg (ap, ast_stms *);
        break;
      }
    case AST_STM_IF2:
      {
        ast_stm_if *get = AST_STM_GET (if, ret);
        get->exp = va_arg (ap, ast_exp *);
        get->then_stms = va_arg (ap, ast_stms *);
        get->else_stms = va_arg (ap, ast_stms *);
        break;
      }
    default:
      error ("unknown stm type %d\n", type);
    }

  va_end (ap);
  return ret;
}

ast_exp *
ast_exp_new (int type, ast_pos pos, ...)
{
  static unsigned sizes[] = {
    [(AST_EXP_ELEM_ST + 1)...(AST_EXP_ELEM_ED - 1)] = AST_EXP_SIZE (elem),
    [AST_EXP_UNARY_UMINUS] = AST_EXP_SIZE (binary),
    [(AST_EXP_BINARY_ST + 1)...(AST_EXP_BINARY_ED - 1)]
    = AST_EXP_SIZE (binary),
  };

  ast_exp *ret = checked_malloc (sizes[type]);
  ret->kind = type;
  ret->pos = pos;

  va_list ap;
  va_start (ap, pos);

  switch (type)
    {
    case AST_EXP_ELEM_ID:
      {
        ast_exp_elem *get = AST_EXP_GET (elem, ret);
        get->id = va_arg (ap, char *);
        break;
      }
    case AST_EXP_ELEM_NUM:
      {
        ast_exp_elem *get = AST_EXP_GET (elem, ret);
        get->num = va_arg (ap, long);
        break;
      }
    case AST_EXP_ELEM_REAL:
      {
        ast_exp_elem *get = AST_EXP_GET (elem, ret);
        get->real = va_arg (ap, double);
        break;
      }
    case AST_EXP_ELEM_STRING:
      {
        ast_exp_elem *get = AST_EXP_GET (elem, ret);
        get->string = va_arg (ap, char *);
        break;
      }
    case AST_EXP_UNARY_UMINUS:
      {
        ast_exp *exp2 = va_arg (ap, ast_exp *);
        ast_exp *exp1 = AST_EXP_NEW (ELEM_NUM, pos, 0);
        ret = AST_EXP_NEW (BINARY_MATH_MINUS, pos, exp1, exp2);
        break;
      }
    case AST_EXP_BINARY_MATH_ST + 1 ... AST_EXP_BINARY_MATH_ED - 1:
    case AST_EXP_BINARY_LOGIC_ST + 1 ... AST_EXP_BINARY_LOGIC_ED - 1:
      {
        ast_exp_binary *get = AST_EXP_GET (binary, ret);
        get->exp1 = va_arg (ap, ast_exp *);
        get->exp2 = va_arg (ap, ast_exp *);
        break;
      }
    default:
      error ("unknown exp type %d\n", type);
    }

  va_end (ap);
  return ret;
}

void
ast_stms_add (ast_stms *stms, ast_stm *stm)
{
  if (stms->size + 1 > stms->cap)
    {
      size_t cap = 2 * stms->cap;
      cap == 0 ? cap = 8 : 0;
      stms->list = realloc (stms->list, cap * sizeof (ast_stm *));
      if (stms->list == NULL)
        error ("realloc failed\n");
      stms->cap = cap;
    }
  stms->list[stms->size++] = stm;
}
