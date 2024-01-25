#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

/* init global env */
void ast_env_init (void);

extern ast_env prog;

/* type: 1 means def, 2 means stm */
ast_env *ast_env_push (ast_env *env, int type, void *ptr);

/* type: 1 means def, 2 means stm */
ast_env *ast_bloc_push (ast_env *env, int type, void *ptr);

ast_def *ast_def_new (int type, ast_pos pos, ...);
ast_stm *ast_stm_new (int type, ast_pos pos, ...);
ast_exp *ast_exp_new (int type, ast_pos pos, ...);

#define AST_DEF_NEW(TYPE, POS, ...)                                           \
  ast_def_new (AST_DEF_##TYPE, POS, ##__VA_ARGS__)
#define AST_STM_NEW(TYPE, POS, ...)                                           \
  ast_stm_new (AST_STM_##TYPE, POS, ##__VA_ARGS__)
#define AST_EXP_NEW(TYPE, POS, ...)                                           \
  ast_exp_new (AST_EXP_##TYPE, POS, ##__VA_ARGS__)

#define AST_DEF_GET(TYPE, PTR) ((ast_def_##TYPE *)((ast_def *)PTR + 1))
#define AST_STM_GET(TYPE, PTR) ((ast_stm_##TYPE *)((ast_stm *)PTR + 1))
#define AST_EXP_GET(TYPE, PTR) ((ast_exp_##TYPE *)((ast_exp *)PTR + 1))

#define AST_DEF_SIZE(TYPE) (sizeof (ast_def) + sizeof (ast_def_##TYPE))
#define AST_STM_SIZE(TYPE) (sizeof (ast_stm) + sizeof (ast_stm_##TYPE))
#define AST_EXP_SIZE(TYPE) (sizeof (ast_exp) + sizeof (ast_exp_##TYPE))

#endif