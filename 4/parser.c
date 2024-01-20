#include "ast.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

ast_stms stms;

extern char *string (const char *str);
extern void *checked_malloc (size_t size);

ast_stm *
ast_stm_assign_new (ast_exp *exp1, ast_exp *exp2, ast_pos pos)
{
  ast_stm *ret = checked_malloc (ast_stm_size (assign));
  ret->kind = AST_STM_ASSIGN;
  ret->pos = pos;

  ast_stm_assign *get = ast_stm_get (assign, ret);
  get->exp1 = exp1;
  get->exp2 = exp2;

  return ret;
}

ast_stm *
ast_stm_while_new (ast_exp *exp, ast_stms *stms, ast_pos pos)
{
  ast_stm *ret = checked_malloc (ast_stm_size (while));
  ret->kind = AST_STM_WHILE;
  ret->pos = pos;

  ast_stm_while *get = ast_stm_get (while, ret);
  get->exp = exp;
  get->stms = stms;

  return ret;
}

ast_stm *
ast_stm_if1_new (ast_exp *exp, ast_stms *then_stms, ast_pos pos)
{
  ast_stm *ret = checked_malloc (ast_stm_size (if));
  ret->kind = AST_STM_IF1;
  ret->pos = pos;

  ast_stm_if *get = ast_stm_get (if, ret);
  get->exp = exp;
  get->then_stms = then_stms;

  return ret;
}

ast_stm *
ast_stm_if2_new (ast_exp *exp, ast_stms *then_stms, ast_stms *else_stms,
                 ast_pos pos)
{
  ast_stm *ret = checked_malloc (ast_stm_size (if));
  ret->kind = AST_STM_IF2;
  ret->pos = pos;

  ast_stm_if *get = ast_stm_get (if, ret);
  get->exp = exp;
  get->then_stms = then_stms;
  get->else_stms = else_stms;

  return ret;
}

ast_exp *
ast_exp_elem_id_new (char *str, ast_pos pos)
{
  ast_exp *ret = checked_malloc (ast_exp_size (elem));
  ret->kind = AST_EXP_ELEM_ID;
  ret->pos = pos;

  ast_exp_elem *get = ast_exp_get (elem, ret);
  get->id = str;

  return ret;
}

ast_exp *
ast_exp_elem_num_new (long num, ast_pos pos)
{
  ast_exp *ret = checked_malloc (ast_exp_size (elem));
  ret->kind = AST_EXP_ELEM_NUM;
  ret->pos = pos;

  ast_exp_elem *get = ast_exp_get (elem, ret);
  get->num = num;

  return ret;
}

ast_exp *
ast_exp_elem_real_new (double real, ast_pos pos)
{
  ast_exp *ret = checked_malloc (ast_exp_size (elem));
  ret->kind = AST_EXP_ELEM_REAL;
  ret->pos = pos;

  ast_exp_elem *get = ast_exp_get (elem, ret);
  get->real = real;

  return ret;
}

ast_exp *
ast_exp_elem_string_new (char *str, ast_pos pos)
{
  ast_exp *ret = checked_malloc (ast_exp_size (elem));
  ret->kind = AST_EXP_ELEM_STRING;
  ret->pos = pos;

  ast_exp_elem *get = ast_exp_get (elem, ret);
  get->string = str;

  return ret;
}

ast_exp *
ast_exp_unary_uminus_new (ast_exp *exp, ast_pos pos)
{
  return ast_exp_binary_math_minus_new (ast_exp_elem_num_new (0, pos), exp,
                                        pos);
}

static inline ast_exp *
ast_exp_binary_new (ast_exp *exp1, ast_exp *exp2)
{
  ast_exp *ret = checked_malloc (ast_exp_size (binary));

  ast_exp_binary *get = ast_exp_get (binary, ret);
  get->exp1 = exp1;
  get->exp2 = exp2;

  return ret;
}

ast_exp *
ast_exp_binary_math_plus_new (ast_exp *exp1, ast_exp *exp2, ast_pos pos)
{
  ast_exp *ret = ast_exp_binary_new (exp1, exp2);
  ret->kind = AST_EXP_BINARY_MATH_PLUS;
  ret->pos = pos;
  return ret;
}

ast_exp *
ast_exp_binary_math_minus_new (ast_exp *exp1, ast_exp *exp2, ast_pos pos)
{
  ast_exp *ret = ast_exp_binary_new (exp1, exp2);
  ret->kind = AST_EXP_BINARY_MATH_MINUS;
  ret->pos = pos;
  return ret;
}

ast_exp *
ast_exp_binary_math_times_new (ast_exp *exp1, ast_exp *exp2, ast_pos pos)
{
  ast_exp *ret = ast_exp_binary_new (exp1, exp2);
  ret->kind = AST_EXP_BINARY_MATH_TIMES;
  ret->pos = pos;
  return ret;
}

ast_exp *
ast_exp_binary_math_div_new (ast_exp *exp1, ast_exp *exp2, ast_pos pos)
{
  ast_exp *ret = ast_exp_binary_new (exp1, exp2);
  ret->kind = AST_EXP_BINARY_MATH_DIV;
  ret->pos = pos;
  return ret;
}

ast_exp *
ast_exp_binary_logic_lt_new (ast_exp *exp1, ast_exp *exp2, ast_pos pos)
{
  ast_exp *ret = ast_exp_binary_new (exp1, exp2);
  ret->kind = AST_EXP_BINARY_LOGIC_LT;
  ret->pos = pos;
  return ret;
}

ast_exp *
ast_exp_binary_logic_gt_new (ast_exp *exp1, ast_exp *exp2, ast_pos pos)
{
  ast_exp *ret = ast_exp_binary_new (exp1, exp2);
  ret->kind = AST_EXP_BINARY_LOGIC_GT;
  ret->pos = pos;
  return ret;
}

ast_exp *
ast_exp_binary_logic_leq_new (ast_exp *exp1, ast_exp *exp2, ast_pos pos)
{
  ast_exp *ret = ast_exp_binary_new (exp1, exp2);
  ret->kind = AST_EXP_BINARY_LOGIC_LEQ;
  ret->pos = pos;
  return ret;
}

ast_exp *
ast_exp_binary_logic_neq_new (ast_exp *exp1, ast_exp *exp2, ast_pos pos)
{
  ast_exp *ret = ast_exp_binary_new (exp1, exp2);
  ret->kind = AST_EXP_BINARY_LOGIC_NEQ;
  ret->pos = pos;
  return ret;
}

ast_exp *
ast_exp_binary_logic_lteq_new (ast_exp *exp1, ast_exp *exp2, ast_pos pos)
{
  ast_exp *ret = ast_exp_binary_new (exp1, exp2);
  ret->kind = AST_EXP_BINARY_LOGIC_LTEQ;
  ret->pos = pos;
  return ret;
}

ast_exp *
ast_exp_binary_logic_gteq_new (ast_exp *exp1, ast_exp *exp2, ast_pos pos)
{
  ast_exp *ret = ast_exp_binary_new (exp1, exp2);
  ret->kind = AST_EXP_BINARY_LOGIC_GTEQ;
  ret->pos = pos;
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
