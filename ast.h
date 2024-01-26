#ifndef AST_H
#define AST_H

#include "util.h"

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
  AST_TYPE_CMPD_ST,
  AST_TYPE_ARRAY,
  AST_TYPE_UNION,
  AST_TYPE_STRUCT,
  AST_TYPE_POINTER,
  AST_TYPE_CMPD_ED,
  AST_TYPE_USER,
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
  AST_EXP_UN_DADDR,
  AST_EXP_UN_ADDR,
  AST_EXP_UN_ED,
  AST_EXP_BIN_ST,
  AST_EXP_BIN_BIT_ST,
  AST_EXP_BIN_AND,
  AST_EXP_BIN_OR,
  AST_EXP_BIN_XOR,
  AST_EXP_BIN_BIT_ED,
  AST_EXP_BIN_MEM_ST,
  AST_EXP_BIN_MEM,
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
typedef struct ast_val ast_val;

typedef struct ast_env ast_env;
typedef struct ast_type ast_type;
typedef struct ast_parm ast_parm;

typedef struct ast_def ast_def;
typedef struct ast_def_var ast_def_var;
typedef struct ast_def_type ast_def_type;
typedef struct ast_def_func ast_def_func;

typedef struct ast_stm ast_stm;
typedef struct ast_stm_if ast_stm_if;
typedef struct ast_stm_while ast_stm_while;
typedef struct ast_stm_assign ast_stm_assign;

typedef struct ast_exp ast_exp;
typedef struct ast_exp_elem ast_exp_elem;
typedef struct ast_exp_unary ast_exp_unary;
typedef struct ast_exp_binary ast_exp_binary;

/* ********************************************** */
/*                    ast pos                     */
/* ********************************************** */

struct ast_pos
{
  unsigned ln;
  unsigned ch;
};

struct ast_tok
{
  int kind;
  ast_pos pos;
  union
  {
    long num;
    void *ptr;
    double real;
  };
};

/* ********************************************** */
/*                    ast env                     */
/* ********************************************** */

struct ast_env
{
  vector defs;
  vector stms;
  ast_env *outer;
};

/* ********************************************** */
/*                    ast type                    */
/* ********************************************** */

struct ast_type
{
  int kind;
  ast_pos pos;
  unsigned size;
  union
  {
    /* array and pointer */
    ast_type *ref;
    /* union and struct */
    ast_env *mem;
  } cmpd;
};

/* ********************************************** */
/*                    ast def                     */
/* ********************************************** */

struct ast_def
{
  int kind;
  ast_pos pos;
  char *id;
} __attribute__ ((aligned (sizeof (size_t))));

struct ast_def_var
{
  ast_type *type;
  ast_exp *init;
};

struct ast_def_type
{
  ast_type *type;
};

struct ast_def_func
{
  ast_type *type;
  ast_env *parm;
  ast_env *env;
};

/* ********************************************** */
/*                    ast stm                     */
/* ********************************************** */

struct ast_stm
{
  int kind;
  ast_pos pos;
} __attribute__ ((aligned (sizeof (size_t))));

struct ast_stm_assign
{
  ast_def *var;
  ast_exp *exp;
};

struct ast_stm_while
{
  ast_exp *exp;
  ast_env *env;
};

struct ast_stm_if
{
  ast_exp *exp;
  ast_env *then_env;
  ast_env *else_env;
};

/* ********************************************** */
/*                    ast exp                     */
/* ********************************************** */

struct ast_exp
{
  int kind;
  ast_pos pos;
} __attribute__ ((aligned (sizeof (size_t))));

struct ast_exp_elem
{
  union
  {
    long num;
    double real;
    ast_def *id;
    char *string;
  };
};

struct ast_exp_unary
{
  ast_exp *exp;
};

struct ast_exp_binary
{
  ast_exp *exp1;
  ast_exp *exp2;
};

struct ast_exp_comma
{
  vector exps;
};

#endif
