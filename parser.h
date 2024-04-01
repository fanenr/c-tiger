#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

extern void ast_prog_init (void);

extern void parser_other (const char *text);

/* set pos from tok */
#define POS(tok) set_parse_pos (tok)

extern void set_parse_pos (ast_tok tok);

/* new env */
#define NEW_ENV() ast_env_new ()

extern void ast_env_new (void);

/* **************************************************************** */
/*                             type new                             */
/* **************************************************************** */

extern ast_type *ast_type1_new (ast_tok name);

extern ast_type *ast_type2_new (ast_type *ref);

/* **************************************************************** */
/*                             def new                              */
/* **************************************************************** */

extern void ast_def_var_new (ast_tok name, ast_type *type);

extern void ast_def_type_new (ast_tok name, ast_type *type);

extern void ast_def_union_new (ast_tok name);

extern void ast_def_struct_new (ast_tok name);

extern void ast_def_func_new (ast_tok name, ast_type *type);

extern void ast_func_parm_new (ast_tok name, ast_type *type);

/* **************************************************************** */
/*                             stm new                              */
/* **************************************************************** */

extern void ast_stm_return_new (ast_exp *val);

extern void ast_stm_assign_new (ast_exp *obj, ast_exp *val);

extern void ast_stm_while_new (ast_exp *cond);

extern void ast_stm_if1_new (ast_exp *cond);

extern void ast_stm_if2_new (ast_stm *base);

/* **************************************************************** */
/*                             exp new                              */
/* **************************************************************** */

#define UN_NEW(kind, exp) ast_exp_unary_new (AST_EXP_UN_##kind, exp)

#define BIN_NEW(kind, exp1, exp2)                                             \
  ast_exp_binary_new (AST_EXP_BIN_##kind, exp1, exp2)

extern ast_exp *ast_exp_elem_new (ast_tok tok);

extern ast_exp *ast_exp_unary_new (int kind, ast_exp *exp);

extern ast_exp *ast_exp_call_new (ast_tok name, array_t *args);

extern array_t *ast_call_args_new (array_t *args, ast_exp *arg);

extern ast_exp *ast_exp_binary_new (int kind, ast_exp *, ast_exp *exp2);

#endif
