%{
#include "parser.h"

extern ast_stms stms;
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
      { ast_stms_add (&stms, $1); }
    | stms stm
      { ast_stms_add (&stms, $2); }
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
      { $$ = AST_STM_NEW (ASSIGN, $2, $1, $3);  }
    ;

stm_while
    : WHILE exp_paren bloc
      { $$ = AST_STM_NEW (WHILE, $1, $2, $3);   }
    ;

stm_if
    : IF exp_paren bloc
      { $$ = AST_STM_NEW (IF1, $1, $2, $3);     }
    | IF exp_paren bloc ELSE bloc
      { $$ = AST_STM_NEW (IF2, $1, $2, $3, $5); }
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
      { $$ = AST_EXP_NEW (ELEM_ID, pos, $1);     }
    | NUM
      { $$ = AST_EXP_NEW (ELEM_NUM, pos, $1);    }
    | REAL
      { $$ = AST_EXP_NEW (ELEM_REAL, pos, $1);   }
    | STRING
      { $$ = AST_EXP_NEW (ELEM_STRING, pos, $1); }
    ;

exp_paren
    : LPAREN exp RPAREN
      { $$ = $2; }
    ;

exp_unary
    : MINUS exp %prec UMINUS
      { $$ = AST_EXP_NEW (UNARY_UMINUS, $1, $2); }
    ;

exp_binary
    : exp_binary_math
      { $$ = $1; }
    | exp_binary_logic
      { $$ = $1; }
    ;

exp_binary_math
    : exp PLUS exp
      { $$ = AST_EXP_NEW (BINARY_MATH_PLUS, $2, $1, $3);  }
    | exp MINUS exp
      { $$ = AST_EXP_NEW (BINARY_MATH_MINUS, $2, $1, $3); }
    | exp TIMES exp
      { $$ = AST_EXP_NEW (BINARY_MATH_TIMES, $2, $1, $3); }
    | exp DIV exp
      { $$ = AST_EXP_NEW (BINARY_MATH_DIV, $2, $1, $3);   }
    ;

exp_binary_logic
    : exp LT exp
      { $$ = AST_EXP_NEW (BINARY_LOGIC_LT, $2, $1, $3);   }
    | exp GT exp
      { $$ = AST_EXP_NEW (BINARY_LOGIC_GT, $2, $1, $3);   }
    | exp LEQ exp
      { $$ = AST_EXP_NEW (BINARY_LOGIC_LEQ, $2, $1, $3);  }
    | exp NEQ exp
      { $$ = AST_EXP_NEW (BINARY_LOGIC_NEQ, $2, $1, $3);  }
    | exp LTEQ exp
      { $$ = AST_EXP_NEW (BINARY_LOGIC_LTEQ, $2, $1, $3); }
    | exp GTEQ exp
      { $$ = AST_EXP_NEW (BINARY_LOGIC_GTEQ, $2, $1, $3); }
    ;
%%
