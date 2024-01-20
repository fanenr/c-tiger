#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

ast_stm *ast_stm_assign_new (ast_exp *exp1, ast_exp *exp2);
ast_stm *ast_stm_while_new (ast_exp *exp, ast_stm *stm);
ast_stm *ast_stm_if1_new (ast_exp *exp, ast_stms *then_stms);
ast_stm *ast_stm_if2_new (ast_exp *exp, ast_stms *then_stms,
                          ast_stms *else_stms);

ast_exp *ast_exp_elem_id_new (const char *str);
ast_exp *ast_exp_elem_num_new (long num);
ast_exp *ast_exp_elem_real_new (double real);
ast_exp *ast_exp_elem_string_new (const char *str);
ast_exp *ast_exp_unary_uminus_new (ast_exp *exp);
ast_exp *ast_exp_binary_math_plus (ast_exp *exp1, ast_exp *exp2);
ast_exp *ast_exp_binary_math_minus (ast_exp *exp1, ast_exp *exp2);
ast_exp *ast_exp_binary_math_times (ast_exp *exp1, ast_exp *exp2);
ast_exp *ast_exp_binary_math_div (ast_exp *exp1, ast_exp *exp2);
ast_exp *ast_exp_binary_logic_lt (ast_exp *exp1, ast_exp *exp2);
ast_exp *ast_exp_binary_logic_gt (ast_exp *exp1, ast_exp *exp2);
ast_exp *ast_exp_binary_logic_leq (ast_exp *exp1, ast_exp *exp2);
ast_exp *ast_exp_binary_logic_neq (ast_exp *exp1, ast_exp *exp2);
ast_exp *ast_exp_binary_logic_lteq (ast_exp *exp1, ast_exp *exp2);
ast_exp *ast_exp_binary_logic_gteq (ast_exp *exp1, ast_exp *exp2);

#endif