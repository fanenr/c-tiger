#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

/* init prog env */
extern void ast_prog_init (void);

/* syntax error */
extern void parser_other (const char *msg);

/* set pos from tok */
#define POS(tok) set_parse_pos (tok)

extern void set_parse_pos (ast_tok tok);

/* new env */
extern ast_env *ast_env_new (void);

/* end env */
extern ast_env *ast_env_end (void);

/* **************************************************************** */
/*                             type new                             */
/* **************************************************************** */

extern ast_type *ast_type1_new (ast_tok name);

extern ast_type *ast_type2_new (ast_type *ref);

/* **************************************************************** */
/*                             def new                              */
/* **************************************************************** */

extern void ast_def_var_new (ast_tok name, ast_type *type);

extern void ast_def_union_new (ast_tok name, ast_env *env);

extern void ast_def_struct_new (ast_tok name, ast_env *env);

extern void ast_def_type_new (ast_tok name, ast_type *type);

extern void ast_func_parm_new (ast_tok name, ast_type *type);

extern void ast_def_func_new (ast_tok name, ast_type *type, ast_env *env);

/* **************************************************************** */
/*                             stm new                              */
/* **************************************************************** */

extern void ast_stm_return_new (ast_exp *exp);

extern void ast_stm_assign_new (ast_exp *obj, ast_exp *exp);

extern void ast_stm_while_new (ast_exp *cond, ast_env *env);

extern void ast_stm_if_new (ast_exp *cond, ast_env *then_env,
			    ast_env *else_env);

/* **************************************************************** */
/*                             exp new                              */
/* **************************************************************** */

extern ast_exp *ast_exp_elem_new (ast_tok tok);

#define UN_NEW(kind, exp) ast_exp_unary_new (AST_EXP_UN_##kind, exp)

extern ast_exp *ast_exp_unary_new (int kind, ast_exp *exp);

extern ast_exp *ast_exp_call_new (ast_tok name, array_t *args);

extern array_t *ast_call_args_new (array_t *args, ast_exp *arg);

extern ast_exp *ast_exp_dmem_new (ast_exp *obj, ast_tok name);

extern ast_exp *ast_exp_pmem_new (ast_exp *obj, ast_tok name);

#define BIN_NEW(kind, exp1, exp2)                                             \
  ast_exp_binary_new (AST_EXP_BIN_##kind, exp1, exp2)

extern ast_exp *ast_exp_binary_new (int kind, ast_exp *, ast_exp *exp2);

#endif
