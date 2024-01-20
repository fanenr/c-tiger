%{
#include "parser.h"

extern int yylex (void);
extern void yyerror(const char *);
%}

%union {
         long num;
         void *ptr;
         double real;
         ast_pos pos;
       }

%token <num> NUM
%token <real> REAL
%token <ptr> ID STRING
%token <pos>
       IF ELSE WHILE
       EQ PLUS MINUS TIMES DIV
       LT GT LEQ NEQ LTEQ GTEQ
       LPAREN RPAREN LBRACE RBRACE

%nonassoc IF
%nonassoc ELSE
%nonassoc EQ
%nonassoc LEQ NEQ
%left LT GT LTEQ GTEQ
%left PLUS MINUS
%left TIMES DIV
%nonassoc UMINUS

%start prog
%type <ptr>
      prog bloc stms stm stm_assign stm_while stm_if
      exp exp_elem exp_paren exp_unary exp_binary 
      exp_binary_math exp_binary_logic

%%
prog
    : stms
      { }
    ;

stms
    : stm
      { stms_add ($1); }
    | stms stm
      { stms_add ($2); }
    ;

stm
    : stm_assign
      { $$ = $1; }
    | stm_while
      { $$ = $1; }
    | stm_if
      { $$ = $1; }
    ;

stm_assign
    : exp EQ exp
      { $$ = ast_stm_assign_new ($1, $3, $2);  }
    ;

stm_while
    : WHILE exp_paren bloc
      { $$ = ast_stm_while_new ($2, $3, $1);   }
    ;

stm_if
    : IF exp_paren bloc
      { $$ = ast_stm_if1_new ($2, $3, $1);     }
    | IF exp_paren bloc ELSE bloc
      { $$ = ast_stm_if2_new ($2, $3, $5, $1); }
    ;

bloc
    : LBRACE stms RBRACE
      { $$ = $2; }
    ;

exp
    : exp_elem
      { $$ = $1; }
    | exp_paren
      { $$ = $1; }
    | exp_unary
      { $$ = $1; }
    | exp_binary
      { $$ = $1; }
    ;

exp_elem
    : ID
      { $$ = ast_exp_elem_id_new ($1, pos);     }
    | NUM
      { $$ = ast_exp_elem_num_new ($1, pos);    }
    | REAL
      { $$ = ast_exp_elem_real_new ($1, pos);   }
    | STRING
      { $$ = ast_exp_elem_string_new ($1, pos); }
    ;

exp_paren
    : LPAREN exp RPAREN
      { $$ = $2; }
    ;

exp_unary
    : MINUS exp %prec UMINUS
      { $$ = ast_exp_unary_uminus_new ($2, $1); }
    ;

exp_binary
    : exp_binary_math
      { $$ = $1; }
    | exp_binary_logic
      { $$ = $1; }
    ;

exp_binary_math
    : exp PLUS exp
      { $$ = ast_exp_binary_math_plus_new ($1, $3, $2);  }
    | exp MINUS exp
      { $$ = ast_exp_binary_math_minus_new ($1, $3, $2); }
    | exp TIMES exp
      { $$ = ast_exp_binary_math_times_new ($1, $3, $2); }
    | exp DIV exp
      { $$ = ast_exp_binary_math_div_new ($1, $3, $2);   }
    ;

exp_binary_logic
    : exp LT exp
      { $$ = ast_exp_binary_logic_lt_new ($1, $3, $2);   }
    | exp GT exp
      { $$ = ast_exp_binary_logic_gt_new ($1, $3, $2);   }
    | exp LEQ exp
      { $$ = ast_exp_binary_logic_leq_new ($1, $3, $2);  }
    | exp NEQ exp
      { $$ = ast_exp_binary_logic_neq_new ($1, $3, $2);  }
    | exp LTEQ exp
      { $$ = ast_exp_binary_logic_lteq_new ($1, $3, $2); }
    | exp GTEQ exp
      { $$ = ast_exp_binary_logic_gteq_new ($1, $3, $2); }
    ;
%%
