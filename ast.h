#ifndef AST_H
#define AST_H

#include "array.h"
#include "mstr.h"

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
  AST_STM_RETURN,
  AST_STM_ASSIGN,
  AST_STM_WHILE,
  AST_STM_IF,
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
  AST_EXP_BIN_ED,
  AST_EXP_CALL,
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
typedef struct ast_exp_call ast_exp_call;
typedef struct ast_exp_unary ast_exp_unary;
typedef struct ast_exp_binary ast_exp_binary;

extern ast_env prog;
extern const char *base_type_name[];
extern const unsigned base_type_size[];

/* **************************************************************** */
/*                             ast pos                              */
/* **************************************************************** */

struct ast_pos
{
  int ln; /* line number */
  int ch; /* char number */
};

struct ast_tok
{
  int kind;    /* kind of tok */
  ast_pos pos; /* pos in source code */
  union
  {
    long num;    /* for integer */
    mstr_t str;  /* for tok and string-literal */
    double real; /* for real number */
  };
};

/* **************************************************************** */
/*                             ast env                              */
/* **************************************************************** */

struct ast_env
{
  array_t stms;   /* stms */
  array_t defs;   /* defs */
  ast_env *outer; /* outer env */
};

/* **************************************************************** */
/*                             ast type                             */
/* **************************************************************** */

struct ast_type
{
  int kind;      /* kind of type */
  ast_pos pos;   /* pos in source code */
  unsigned size; /* size of type */
  union
  {
    mstr_t name;   /* for undetermined type */
    ast_env *mem;  /* for union or struct */
    ast_type *ref; /* for pointer */
  };
};

/* **************************************************************** */
/*                             ast def                              */
/* **************************************************************** */

struct ast_def
{
  int kind;    /* kind of def */
  int index;   /* index in env */
  ast_pos pos; /* pos in source code */
  mstr_t name; /* name of def */
};

struct ast_def_var
{
  ast_def base;
  ast_type *type; /* kind of var */
};

struct ast_def_type
{
  ast_def base;
  ast_type *type; /* origin type */
};

struct ast_def_func
{
  ast_def base;
  ast_env *env;   /* func body */
  ast_type *type; /* return type of func */
  unsigned parms; /* the number of func parms  */
};

/* **************************************************************** */
/*                             ast stm                              */
/* **************************************************************** */

struct ast_stm
{
  int kind;    /* kind of stm */
  int index;   /* index in env */
  ast_pos pos; /* pos in source code */
};

struct ast_stm_return
{
  ast_stm base;
  ast_exp *val; /* return value */
};

struct ast_stm_assign
{
  ast_stm base;
  ast_exp *obj; /* assign object */
  ast_exp *val; /* assign value */
};

struct ast_stm_while
{
  ast_stm base;
  ast_env *env;  /* while body */
  ast_exp *cond; /* while condition */
};

struct ast_stm_if
{
  ast_stm base;
  ast_exp *cond;     /* if condition */
  ast_env *then_env; /* then body */
  ast_env *else_env; /* else body */
};

/* **************************************************************** */
/*                             ast exp                              */
/* **************************************************************** */

struct ast_exp
{
  int kind;       /* kind of exp */
  ast_pos pos;    /* pos in source code */
  ast_type *type; /* type of exp */
};

struct ast_exp_elem
{
  ast_exp base;
  union
  {
    long num;         /* integer-literal */
    mstr_t str;       /* string-literal */
    double real;      /* real-literal */
    ast_def_var *var; /* variable */
  };
};

struct ast_exp_call
{
  array_t *args;      /* args */
  ast_def_func *func; /* target  */
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

/* **************************************************************** */
/*                               util                               */
/* **************************************************************** */

#include <stdio.h>

#define ast_error(POS, FMT, ...)                                              \
  do                                                                          \
    {                                                                         \
      fprintf (stderr, "error occured at %d:%d: ", (POS).ln, (POS).ch);       \
      fprintf (stderr, FMT, ##__VA_ARGS__);                                   \
      fprintf (stderr, "\n");                                                 \
      __builtin_trap ();                                                      \
    }                                                                         \
  while (0)

#endif
