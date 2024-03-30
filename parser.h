#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

extern void ast_prog_init (void);

#define POS(tok) set_parse_pos (tok)

/* set pos from tok */
extern void set_parse_pos (ast_tok tok);

/* push a stm to env */
extern ast_env *ast_env_push_stm (ast_env *env, ast_stm *stm);

/* push a def to env */
extern ast_env *ast_env_push_def (ast_env *env, ast_def *def);

/* **************************************************************** */
/*                             type new                             */
/* **************************************************************** */

extern ast_type *ast_type_new (ast_type *type, ast_tok tok);

/* **************************************************************** */
/*                             def new                              */
/* **************************************************************** */

extern ast_def *ast_def_var_new (ast_tok name, ast_type *type);

extern ast_def *ast_def_type_new (ast_tok name, ast_type *origin);

extern ast_def *ast_def_type_union_new (ast_tok name, ast_env *env);

extern ast_def *ast_def_type_struct_new (ast_tok name, ast_env *env);

extern array_t *ast_def_func_parm_new (array_t *parm, ast_tok name,
                                       ast_type *type);

extern ast_def *ast_def_func_new (ast_tok name, array_t *parm, ast_type *type,
                                  ast_env *env);

/* **************************************************************** */
/*                             stm new                              */
/* **************************************************************** */

extern ast_stm *ast_stm_return_new (ast_exp *val);

extern ast_stm *ast_stm_assign_new (ast_exp *obj, ast_exp *val);

extern ast_stm *ast_stm_while_new (ast_exp *cond, ast_env *env);

extern ast_stm *ast_stm_if_new (ast_exp *cond, ast_env *then_env,
                                ast_env *else_env);

/* **************************************************************** */
/*                             exp new                              */
/* **************************************************************** */

#define UN_NEW(kind, exp) ast_exp_unary_new (AST_EXP_UN_##kind, exp)

#define BIN_NEW(kind, exp1, exp2)                                             \
  ast_exp_binary_new (AST_EXP_BIN_##kind, exp1, exp2)

extern ast_exp *ast_exp_elem_new (ast_tok tok);

extern ast_exp *ast_exp_unary_new (int kind, ast_exp *exp);

extern ast_exp *ast_exp_call_new (ast_tok name, array_t *args);

extern array_t *ast_exp_call_args_new (array_t *args, ast_exp *arg);

extern ast_exp *ast_exp_binary_new (int kind, ast_exp *, ast_exp *exp2);

#endif
