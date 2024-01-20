#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

/* from lexer */
extern ast_pos pos;

ast_stm *ast_stm_new (int type, ast_pos pos, ...);
ast_exp *ast_exp_new (int type, ast_pos pos, ...);

#define AST_STM_NEW(TYPE, POS, ...)                                           \
  ast_stm_new (AST_STM_##TYPE, POS, ##__VA_ARGS__)
#define AST_EXP_NEW(TYPE, POS, ...)                                           \
  ast_exp_new (AST_EXP_##TYPE, POS, ##__VA_ARGS__)

#define AST_STM_GET(TYPE, PTR) ((ast_stm_##TYPE *)(PTR + 1))
#define AST_EXP_GET(TYPE, PTR) ((ast_exp_##TYPE *)(PTR + 1))
#define AST_STM_SIZE(TYPE) (sizeof (ast_stm) + sizeof (ast_stm_##TYPE))
#define AST_EXP_SIZE(TYPE) (sizeof (ast_exp) + sizeof (ast_exp_##TYPE))

void ast_stms_add (ast_stms *stms, ast_stm *stm)
    __attribute__ ((nonnull (1, 2)));

#endif