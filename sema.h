#ifndef SEMA_H
#define SEMA_H

#include "ast.h"

extern void sema_check (ast_env *env);

extern void sema_check_def (ast_def *def, ast_env *env);
extern void sema_check_exp (ast_exp *exp, ast_env *env);

#endif
