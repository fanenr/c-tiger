#ifndef AST_H
#define AST_H

enum
{
  AST_STM_ST,
  AST_STM_ASSIGN,
  AST_STM_WHILE,
  AST_STM_IF_ST,
  AST_STM_IF1,
  AST_STM_IF2,
  AST_STM_IF_ED,
  AST_STM_ED,
  AST_EXP_ST,
  AST_EXP_ELEM_ST,
  AST_EXP_ELEM_ID,
  AST_EXP_ELEM_NUM,
  AST_EXP_ELEM_REAL,
  AST_EXP_ELEM_STRING,
  AST_EXP_ELEM_ED,
  AST_EXP_BINARY_ST,
  AST_EXP_BINARY_MATH_ST,
  AST_EXP_BINARY_MATH_PLUS,
  AST_EXP_BINARY_MATH_MINUS,
  AST_EXP_BINARY_MATH_TIMES,
  AST_EXP_BINARY_MATH_DIV,
  AST_EXP_BINARY_MATH_ED,
  AST_EXP_BINARY_LOGIC_ST,
  AST_EXP_BINARY_LOGIC_LT,
  AST_EXP_BINARY_LOGIC_GT,
  AST_EXP_BINARY_LOGIC_LEQ,
  AST_EXP_BINARY_LOGIC_NEQ,
  AST_EXP_BINARY_LOGIC_LTEQ,
  AST_EXP_BINARY_LOGIC_GTEQ,
  AST_EXP_BINARY_LOGIC_ED,
  AST_EXP_BINARY_ED,
  AST_EXP_ED,
};

typedef struct ast_stms ast_stms;

typedef struct ast_stm ast_stm;
typedef struct ast_stm_assign ast_stm_assign;
typedef struct ast_stm_while ast_stm_while;
typedef struct ast_stm_if ast_stm_if;

typedef struct ast_exp ast_exp;
typedef struct ast_exp_elem ast_exp_elem;
typedef struct ast_exp_binary ast_exp_binary;

struct ast_stms
{
  unsigned cap;
  unsigned size;
  ast_stm **list;
};

struct ast_stm
{
  unsigned kind;
  unsigned lnpos;
  unsigned chpos;
} __attribute__ ((aligned (sizeof (void *))));

struct ast_stm_assign
{
  ast_exp *exp1;
  ast_exp *exp2;
};

struct ast_stm_while
{
  ast_exp *exp;
  ast_stms *stms;
};

struct ast_stm_if
{
  ast_exp *exp;
  ast_stms *then_stms;
  ast_stms *else_stms;
};

struct ast_exp
{
  unsigned kind;
  unsigned lnpos;
  unsigned chpos;
} __attribute__ ((aligned (sizeof (void *))));

struct ast_exp_elem
{
  union
  {
    long num;
    char *id;
    double real;
    char *string;
  };
};

struct ast_exp_binary
{
  ast_exp *exp1;
  ast_exp *exp2;
};

#endif
