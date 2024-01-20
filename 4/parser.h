#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

/* from lexer */
extern ast_pos pos;

ast_stm *ast_stm_assign_new (ast_exp *exp1, ast_exp *exp2, ast_pos pos);
ast_stm *ast_stm_while_new (ast_exp *exp, ast_stms *stms, ast_pos pos);
ast_stm *ast_stm_if1_new (ast_exp *exp, ast_stms *then_stms, ast_pos pos);
ast_stm *ast_stm_if2_new (ast_exp *exp, ast_stms *then_stms,
                          ast_stms *else_stms, ast_pos pos);

ast_exp *ast_exp_elem_id_new (char *str, ast_pos pos);
ast_exp *ast_exp_elem_num_new (long num, ast_pos pos);
ast_exp *ast_exp_elem_real_new (double real, ast_pos pos);
ast_exp *ast_exp_elem_string_new (char *str, ast_pos pos);
ast_exp *ast_exp_unary_uminus_new (ast_exp *exp, ast_pos pos);

ast_exp *ast_exp_binary_math_plus_new (ast_exp *exp1, ast_exp *exp2,
                                       ast_pos pos);
ast_exp *ast_exp_binary_math_minus_new (ast_exp *exp1, ast_exp *exp2,
                                        ast_pos pos);
ast_exp *ast_exp_binary_math_times_new (ast_exp *exp1, ast_exp *exp2,
                                        ast_pos pos);
ast_exp *ast_exp_binary_math_div_new (ast_exp *exp1, ast_exp *exp2,
                                      ast_pos pos);

ast_exp *ast_exp_binary_logic_lt_new (ast_exp *exp1, ast_exp *exp2,
                                      ast_pos pos);
ast_exp *ast_exp_binary_logic_gt_new (ast_exp *exp1, ast_exp *exp2,
                                      ast_pos pos);
ast_exp *ast_exp_binary_logic_leq_new (ast_exp *exp1, ast_exp *exp2,
                                       ast_pos pos);
ast_exp *ast_exp_binary_logic_neq_new (ast_exp *exp1, ast_exp *exp2,
                                       ast_pos pos);
ast_exp *ast_exp_binary_logic_lteq_new (ast_exp *exp1, ast_exp *exp2,
                                        ast_pos pos);
ast_exp *ast_exp_binary_logic_gteq_new (ast_exp *exp1, ast_exp *exp2,
                                        ast_pos pos);

#define ast_stm_get(TYPE, PTR) ((ast_stm_##TYPE *)(PTR + 1))
#define ast_exp_get(TYPE, PTR) ((ast_exp_##TYPE *)(PTR + 1))
#define ast_stm_size(TYPE) (sizeof (ast_stm) + sizeof (ast_stm_##TYPE))
#define ast_exp_size(TYPE) (sizeof (ast_exp) + sizeof (ast_exp_##TYPE))

extern ast_stms stms;
void stms_add (ast_stm *stm);

#endif