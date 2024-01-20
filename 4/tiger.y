%{
#include <stdio.h>

extern int yylex (void);
extern void yyerror(const char *);
%}

%union {
         long num;
         void *ptr;
         double real;
       }

%token <num> NUM
%token <real> REAL
%token <ptr> ID STRING
%token IF ELSE WHILE
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
      { $$ = $1; }
    ;

bloc
    : LBRACE stms RBRACE
      { $$ = $2; }
    ;

stms
    : stm
      { }
    | stms stm
      { }
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
      { }
    ;

stm_while
    : WHILE exp_paren bloc
      { }
    ;

stm_if
    : IF exp_paren bloc
      { }
    | IF exp_paren bloc ELSE bloc
      { }
    ;

exp
    : exp_elem
      { $$ = $1; }
    | exp_unary
      { $$ = $1; }
    | exp_binary
      { $$ = $1; }
    ;

exp_elem
    : ID
      { }
    | NUM
      { }
    | REAL
      { }
    | STRING
      { }
    ;

exp_paren
    : LPAREN exp RPAREN
      { }
    ;

exp_unary
    : MINUS exp %prec UMINUS
      { }
    ;

exp_binary
    : exp_binary_math
      { $$ = $1; }
    | exp_binary_logic
      { $$ = $1; }
    ;

exp_binary_math
    : exp PLUS exp
      { }
    | exp MINUS exp
      { }
    | exp TIMES exp
      { }
    | exp DIV exp
      { }
    ;

exp_binary_logic
    : exp LT exp
      { }
    | exp GT exp
      { }
    | exp LEQ exp
      { }
    | exp NEQ exp
      { }
    | exp LTEQ exp
      { }
    | exp GTEQ exp
      { }
    ;
%%
