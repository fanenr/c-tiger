#ifndef AST_H
#define AST_H

enum
{
  AST_TYPE_ST,
  AST_TYPE_ELEM_ST,
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
  AST_TYPE_ELEM_ED,
  AST_TYPE_COMP_ST,
  AST_TYPE_ARRAY,
  AST_TYPE_UNION,
  AST_TYPE_STRUCT,
  AST_TYPE_POINTER,
  AST_TYPE_COMP_ED,
  AST_TYPE_ED,

  AST_DEF_ST,
  AST_DEF_VAR,
  AST_DEF_TYPE,
  AST_DEF_FUNC,
  AST_DEF_ED,

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
  AST_EXP_ELEM_STR,
  AST_EXP_ELEM_REAL,
  AST_EXP_ELEM_ED,
  AST_EXP_UNARY_ST,
  AST_EXP_UNARY_UMINUS,
  AST_EXP_UNARY_ED,
  AST_EXP_BINARY_ST,
  AST_EXP_BINARY_MATH_ST,
  AST_EXP_BINARY_PLUS,
  AST_EXP_BINARY_MINUS,
  AST_EXP_BINARY_TIMES,
  AST_EXP_BINARY_DIV,
  AST_EXP_BINARY_MATH_ED,
  AST_EXP_BINARY_LOGIC_ST,
  AST_EXP_BINARY_LT,
  AST_EXP_BINARY_GT,
  AST_EXP_BINARY_LEQ,
  AST_EXP_BINARY_NEQ,
  AST_EXP_BINARY_LTEQ,
  AST_EXP_BINARY_GTEQ,
  AST_EXP_BINARY_LOGIC_ED,
  AST_EXP_BINARY_ED,
  AST_EXP_ED,
};

typedef struct ast_pos
{
  unsigned ln;
  unsigned ch;
} ast_pos;

typedef struct ast_types ast_types;
typedef struct ast_type ast_type;
typedef struct ast_type_array ast_type_array;
typedef struct ast_type_union ast_type_union;
typedef struct ast_type_struct ast_type_struct;

typedef struct ast_defs ast_defs;
typedef struct ast_def ast_def;
typedef struct ast_def_var ast_def_var;
typedef struct ast_def_type ast_def_type;
typedef struct ast_def_func ast_def_func;

typedef struct ast_stms ast_stms;
typedef struct ast_stm ast_stm;
typedef struct ast_stm_assign ast_stm_assign;
typedef struct ast_stm_while ast_stm_while;
typedef struct ast_stm_if ast_stm_if;

typedef struct ast_exp ast_exp;
typedef struct ast_exp_elem ast_exp_elem;
typedef struct ast_exp_binary ast_exp_binary;

typedef struct ast_env ast_env;

/* ********************************************** */
/*                    ast def                     */
/* ********************************************** */

struct ast_defs
{
  unsigned cap;
  unsigned size;
  ast_def **list;
};

struct ast_def
{
  unsigned kind;
  ast_pos pos;
  char *id;
} __attribute__ ((aligned (sizeof (void *))));

struct ast_def_var
{
  ast_def *type;
  ast_exp *init;
};

struct ast_def_type
{
  unsigned comp;
  union
  {
    ast_def *origin;
    ast_def **list;
  };
};

struct ast_def_func
{
  ast_env *env;
  ast_def *type;
  ast_defs *parm;
};

/* ********************************************** */
/*                    ast stm                     */
/* ********************************************** */

struct ast_stms
{
  unsigned cap;
  unsigned size;
  ast_stm **list;
};

struct ast_stm
{
  unsigned kind;
  ast_pos pos;
} __attribute__ ((aligned (sizeof (void *))));

struct ast_stm_assign
{
  ast_def *id;
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
  unsigned kind;
  ast_pos pos;
} __attribute__ ((aligned (sizeof (void *))));

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

struct ast_exp_binary
{
  ast_exp *exp1;
  ast_exp *exp2;
};

/* ********************************************** */
/*                    ast env                     */
/* ********************************************** */

struct ast_env
{
  ast_defs defs;
  ast_stms stms;
  ast_env *outer;
};

#endif
