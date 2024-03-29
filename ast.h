#ifndef AST_H
#define AST_H

#include "array.h"
#include "mstr.h"
#include "rbtree.h"

enum
{
  AST_TYPE_ST,
  AST_TYPE_BASE_ST,
  AST_TYPE_VOID,
  AST_TYPE_BOOL,
  AST_TYPE_INT8,
  AST_TYPE_INT16,
  AST_TYPE_INT32,
  AST_TYPE_INT64,
  AST_TYPE_UINT8,
  AST_TYPE_UINT16,
  AST_TYPE_UINT32,
  AST_TYPE_UINT64,
  AST_TYPE_FLOAT,
  AST_TYPE_DOUBLE,
  AST_TYPE_BASE_ED,
  AST_TYPE_POINTER,
  AST_TYPE_STRUCT,
  AST_TYPE_UNION,
  AST_TYPE_ED,

  AST_DEF_ST,
  AST_DEF_VAR,
  AST_DEF_TYPE,
  AST_DEF_FUNC,
  AST_DEF_ED,

  AST_STM_ST,
  AST_STM_IF_ST,
  AST_STM_IF1,
  AST_STM_IF2,
  AST_STM_IF_ED,
  AST_STM_WHILE,
  AST_STM_ASSIGN,
  AST_STM_RETURN,
  AST_STM_ED,

  AST_EXP_ST,
  AST_EXP_ELEM_ST,
  AST_EXP_ELEM_ID,
  AST_EXP_ELEM_NUM,
  AST_EXP_ELEM_STR,
  AST_EXP_ELEM_REAL,
  AST_EXP_ELEM_ED,
  AST_EXP_UN_ST,
  AST_EXP_UN_UPLUS,
  AST_EXP_UN_UMINUS,
  AST_EXP_UN_DREF,
  AST_EXP_UN_ADDR,
  AST_EXP_UN_ED,
  AST_EXP_BIN_ST,
  AST_EXP_BIN_BIT_ST,
  AST_EXP_BIN_OR,
  AST_EXP_BIN_AND,
  AST_EXP_BIN_XOR,
  AST_EXP_BIN_BIT_ED,
  AST_EXP_BIN_MEM_ST,
  AST_EXP_BIN_DMEM,
  AST_EXP_BIN_PMEM,
  AST_EXP_BIN_INDEX,
  AST_EXP_BIN_MEM_ED,
  AST_EXP_BIN_MATH_ST,
  AST_EXP_BIN_PLUS,
  AST_EXP_BIN_MINUS,
  AST_EXP_BIN_TIMES,
  AST_EXP_BIN_DIV,
  AST_EXP_BIN_MOD,
  AST_EXP_BIN_MATH_ED,
  AST_EXP_BIN_LOGIC_ST,
  AST_EXP_BIN_LT,
  AST_EXP_BIN_GT,
  AST_EXP_BIN_LEQ,
  AST_EXP_BIN_NEQ,
  AST_EXP_BIN_LTEQ,
  AST_EXP_BIN_GTEQ,
  AST_EXP_BIN_LAND,
  AST_EXP_BIN_LOR,
  AST_EXP_BIN_LOGIC_ED,
  AST_EXP_BIN_CALL,
  AST_EXP_BIN_ED,
  AST_EXP_COMMA,
  AST_EXP_ED,
};

typedef struct ast_pos ast_pos;
typedef struct ast_tok ast_tok;

typedef struct ast_env ast_env;
typedef struct ast_type ast_type;

typedef struct ast_def ast_def;
typedef struct ast_def_var ast_def_var;
typedef struct ast_def_type ast_def_type;
typedef struct ast_def_func ast_def_func;

typedef struct ast_stm ast_stm;
typedef struct ast_stm_if ast_stm_if;
typedef struct ast_stm_while ast_stm_while;
typedef struct ast_stm_assign ast_stm_assign;
typedef struct ast_stm_return ast_stm_return;

typedef struct ast_exp ast_exp;
typedef struct ast_exp_elem ast_exp_elem;
typedef struct ast_exp_unary ast_exp_unary;
typedef struct ast_exp_comma ast_exp_comma;
typedef struct ast_exp_binary ast_exp_binary;

/* **************************************************************** */
/*                             ast pos                              */
/* **************************************************************** */

struct ast_pos
{
  int ln;
  int ch;
};

struct ast_tok
{
  int kind;
  ast_pos pos;
  union
  {
    long num;
    mstr_t str;
    double real;
  };
};

/* **************************************************************** */
/*                             ast env                              */
/* **************************************************************** */

struct ast_env
{
  array_t stms;
  rbtree_t defs;
  ast_env *outer;
};

/* **************************************************************** */
/*                             ast type                             */
/* **************************************************************** */

struct ast_type
{
  int kind;
  ast_pos pos;
  unsigned size;
  union
  {
    ast_env *mem;
    ast_type *ref;
  };
};

/* **************************************************************** */
/*                             ast def                              */
/* **************************************************************** */

struct ast_def
{
  int kind;
  ast_pos pos;
  mstr_t name;
};

struct ast_def_var
{
  ast_def base;
  ast_type *type;
};

struct ast_def_type
{
  ast_def base;
  ast_type *type;
};

struct ast_def_func
{
  ast_def base;
  ast_env *env;
  ast_type *type;
  unsigned parms;
};

/* **************************************************************** */
/*                             ast stm                              */
/* **************************************************************** */

struct ast_stm
{
  int kind;
  ast_pos pos;
};

struct ast_stm_assign
{
  ast_stm base;
  ast_exp *obj;
  ast_exp *exp;
};

struct ast_stm_return
{
  ast_stm base;
  ast_exp *exp;
};

struct ast_stm_while
{
  ast_stm base;
  ast_exp *exp;
  ast_env *env;
};

struct ast_stm_if
{
  ast_stm base;
  ast_exp *exp;
  ast_env *then_env;
  ast_env *else_env;
};

/* **************************************************************** */
/*                             ast exp                              */
/* **************************************************************** */

struct ast_exp
{
  int kind;
  ast_pos pos;
  ast_type *type;
};

struct ast_exp_elem
{
  ast_exp base;
  union
  {
    long num;
    mstr_t str;
    double real;
    ast_def *id;
  };
};

struct ast_exp_unary
{
  ast_exp base;
  ast_exp *exp;
};

struct ast_exp_binary
{
  ast_exp base;
  ast_exp *exp1;
  ast_exp *exp2;
};

struct ast_exp_comma
{
  ast_exp base;
  array_t exps;
};

/* **************************************************************** */
/*                               util                               */
/* **************************************************************** */

#include <stdio.h>

#define ast_error(FMT, POS, ...)                                              \
  do                                                                          \
    {                                                                         \
      fprintf (stderr, "error occured at %d:%d: ", (POS).ln, (POS).ch);       \
      fprintf (stderr, FMT, ##__VA_ARGS__);                                   \
      fprintf (stderr, "\n");                                                 \
      __builtin_trap ();                                                      \
    }                                                                         \
  while (0)

#endif
