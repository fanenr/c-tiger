#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

ast_stms stms;

extern char *string (const char *str);
extern void *checked_malloc (size_t size);

#define set_pos(PTR)                                                          \
  PTR->lnpos = lnpos;                                                         \
  PTR->chpos = chpos;

ast_stm *
ast_stm_assign_new (ast_exp *exp1, ast_exp *exp2)
{
  ast_stm *ret = checked_malloc (ast_stm_size (assign));
  ret->kind = AST_STM_ASSIGN;
  set_pos (ret);

  ast_stm_assign *get = ast_stm_get (assign, ret);
  get->exp1 = exp1;
  get->exp2 = exp2;

  return ret;
}

ast_stm *
ast_stm_while_new (ast_exp *exp, ast_stms *stms)
{
  ast_stm *ret = checked_malloc (ast_stm_size (while));
  ret->kind = AST_STM_WHILE;
  set_pos (ret);

  ast_stm_while *get = ast_stm_get (while, ret);
  get->exp = exp;
  get->stms = stms;

  return ret;
}

ast_stm *
ast_stm_if1_new (ast_exp *exp, ast_stms *then_stms)
{
  ast_stm *ret = checked_malloc (ast_stm_size (if));
  ret->kind = AST_STM_IF1;
  set_pos (ret);

  ast_stm_if *get = ast_stm_get (if, ret);
  get->exp = exp;
  get->then_stms = then_stms;

  return ret;
}

ast_stm *
ast_stm_if2_new (ast_exp *exp, ast_stms *then_stms, ast_stms *else_stms)
{
  ast_stm *ret = checked_malloc (ast_stm_size (if));
  ret->kind = AST_STM_IF2;
  set_pos (ret);

  ast_stm_if *get = ast_stm_get (if, ret);
  get->exp = exp;
  get->then_stms = then_stms;
  get->else_stms = else_stms;

  return ret;
}

ast_exp *
ast_exp_elem_id_new (char *str)
{
  ast_exp *ret = checked_malloc (ast_exp_size (elem));
  ret->kind = AST_EXP_ELEM_ID;
  set_pos (ret);

  ast_exp_elem *get = ast_exp_get (elem, ret);
  get->id = str;

  return ret;
}

ast_exp *
ast_exp_elem_num_new (long num)
{
  ast_exp *ret = checked_malloc (ast_exp_size (elem));
  ret->kind = AST_EXP_ELEM_NUM;
  set_pos (ret);

  ast_exp_elem *get = ast_exp_get (elem, ret);
  get->num = num;

  return ret;
}

ast_exp *
ast_exp_elem_real_new (double real)
{
  ast_exp *ret = checked_malloc (ast_exp_size (elem));
  ret->kind = AST_EXP_ELEM_REAL;
  set_pos (ret);

  ast_exp_elem *get = ast_exp_get (elem, ret);
  get->real = real;

  return ret;
}

ast_exp *
ast_exp_elem_string_new (char *str)
{
  ast_exp *ret = checked_malloc (ast_exp_size (elem));
  ret->kind = AST_EXP_ELEM_STRING;
  set_pos (ret);

  ast_exp_elem *get = ast_exp_get (elem, ret);
  get->string = str;

  return ret;
}

ast_exp *
ast_exp_unary_uminus_new (ast_exp *exp)
{
  return ast_exp_binary_math_minus_new (ast_exp_elem_num_new (0), exp);
}

static inline ast_exp *
ast_exp_binary_new (ast_exp *exp1, ast_exp *exp2)
{
  ast_exp *ret = checked_malloc (ast_exp_size (binary));
  set_pos (ret);

  ast_exp_binary *get = ast_exp_get (binary, ret);
  get->exp1 = exp1;
  get->exp2 = exp2;

  return ret;
}

ast_exp *
ast_exp_binary_math_plus_new (ast_exp *exp1, ast_exp *exp2)
{
  ast_exp *ret = ast_exp_binary_new (exp1, exp2);
  ret->kind = AST_EXP_BINARY_MATH_PLUS;
  return ret;
}

ast_exp *
ast_exp_binary_math_minus_new (ast_exp *exp1, ast_exp *exp2)
{
  ast_exp *ret = ast_exp_binary_new (exp1, exp2);
  ret->kind = AST_EXP_BINARY_MATH_MINUS;
  return ret;
}

ast_exp *
ast_exp_binary_math_times_new (ast_exp *exp1, ast_exp *exp2)
{
  ast_exp *ret = ast_exp_binary_new (exp1, exp2);
  ret->kind = AST_EXP_BINARY_MATH_TIMES;
  return ret;
}

ast_exp *
ast_exp_binary_math_div_new (ast_exp *exp1, ast_exp *exp2)
{
  ast_exp *ret = ast_exp_binary_new (exp1, exp2);
  ret->kind = AST_EXP_BINARY_MATH_DIV;
  return ret;
}

ast_exp *
ast_exp_binary_logic_lt_new (ast_exp *exp1, ast_exp *exp2)
{
  ast_exp *ret = ast_exp_binary_new (exp1, exp2);
  ret->kind = AST_EXP_BINARY_LOGIC_LT;
  return ret;
}

ast_exp *
ast_exp_binary_logic_gt_new (ast_exp *exp1, ast_exp *exp2)
{
  ast_exp *ret = ast_exp_binary_new (exp1, exp2);
  ret->kind = AST_EXP_BINARY_LOGIC_GT;
  return ret;
}

ast_exp *
ast_exp_binary_logic_leq_new (ast_exp *exp1, ast_exp *exp2)
{
  ast_exp *ret = ast_exp_binary_new (exp1, exp2);
  ret->kind = AST_EXP_BINARY_LOGIC_LEQ;
  return ret;
}

ast_exp *
ast_exp_binary_logic_neq_new (ast_exp *exp1, ast_exp *exp2)
{
  ast_exp *ret = ast_exp_binary_new (exp1, exp2);
  ret->kind = AST_EXP_BINARY_LOGIC_NEQ;
  return ret;
}

ast_exp *
ast_exp_binary_logic_lteq_new (ast_exp *exp1, ast_exp *exp2)
{
  ast_exp *ret = ast_exp_binary_new (exp1, exp2);
  ret->kind = AST_EXP_BINARY_LOGIC_LTEQ;
  return ret;
}

ast_exp *
ast_exp_binary_logic_gteq_new (ast_exp *exp1, ast_exp *exp2)
{
  ast_exp *ret = ast_exp_binary_new (exp1, exp2);
  ret->kind = AST_EXP_BINARY_LOGIC_GTEQ;
  return ret;
}

void
stms_add (ast_stm *stm)
{
  if (stms.size + 1 > stms.cap)
    {
      size_t cap = stms.cap == 0 ? 8 : stms.cap * 2;
      stms.list = realloc (stms.list, cap);
      if (stms.list == NULL)
        {
          fprintf (stderr, "cannot append stm to stms\n");
          exit (1);
        }
      stms.cap = cap;
    }
  stms.list[stms.size++] = stm;
}
