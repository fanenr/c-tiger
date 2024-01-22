#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

/* from lexer */
extern ast_pos pos;

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

ast_def *ast_id_seek (const char *name, ast_pos pos);

void ast_stms_add (ast_stms *stms, ast_stm *stm)
    __attribute__ ((nonnull (1, 2)));

void ast_defs_add (ast_defs *defs, ast_def *def)
    __attribute__ ((nonnull (1, 2)));

void ast_env_init (void);
void ast_env_new (void);
void ast_env_add (void *ptr);

#define AST_BLOC_NEW(PTR)                                                     \
  ast_env_new ();                                                             \
  ast_env_add (PTR);                                                          \
  switch (*(int *)PTR)                                                        \
    {                                                                         \
    case AST_DEF_ST ... AST_DEF_ED:                                           \
      env->outer->defs.size--;                                                \
      break;                                                                  \
    case AST_STM_ST ... AST_STM_ED:                                           \
      env->outer->stms.size--;                                                \
      break;                                                                  \
    }

void ast_func_parm_push (ast_pos pos, char *type);
ast_defs *ast_func_parm_pop (void);

extern ast_env *env;
extern ast_defs func_parm;

#endif