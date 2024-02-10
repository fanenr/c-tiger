#ifndef SEMA_H
#define SEMA_H

#include "ast.h"
#include <stdbool.h>

extern void sema_check (ast_env *env);

extern void sema_check_def (ast_def *def, ast_env *env);
extern void sema_check_stm (ast_stm *stm, ast_env *env);
extern void sema_check_exp (ast_exp *exp, ast_env *env);

extern void sema_check_id (ast_tok id, ast_env *env);
extern void sema_check_type (ast_type *type, ast_env *env);

extern bool sema_exp_is_lv (ast_exp *exp);
extern bool sema_exp_is_obj (ast_exp *exp);
extern bool sema_exp_is_num (ast_exp *exp);
extern bool sema_exp_is_int (ast_exp *exp);
extern bool sema_exp_is_real (ast_exp *exp);
extern bool sema_exp_is_pobj (ast_exp *exp);

extern int sema_pos_comp (ast_pos p1, ast_pos p2);
extern ast_def *sema_seek_def1 (const char *name, ast_env *env);
extern ast_def *sema_seek_def2 (const char *name, ast_env *env);

#endif
