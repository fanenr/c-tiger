%{
#include <stdio.h>

extern int yylex (void);
extern void yyerror(const char *);
%}

%start prog

%token IF ELSE WHILE
%token ID NUM REAL STRING
%token EQ PLUS MINUS TIMES DIV
%token LT GT DEQ NEQ LTEQ GTEQ
%token LPAREN RPAREN LBRACE RBRACE

%nonassoc IF
%nonassoc ELSE

%nonassoc EQ
%nonassoc DEQ NEQ
%left LT GT LTEQ GTEQ
%left PLUS MINUS
%left TIMES DIV
%nonassoc UMINUS

%union {
         long num;
         char *str;
         double real;
       }

%%
prog
    : stms                               { }
    ;

bloc
    : LBRACE stms RBRACE                 { }

stms
    : stm                                { }
    | stms stm                           { }
    ;

stm
    : stm_assign
    | stm_while
    | stm_if
    ;

stm_assign
    : ID EQ exp                          { }
    ;

stm_while
    : WHILE exp_paren stm                { }
    | WHILE exp_paren bloc               { }
    ;

stm_if
    : IF exp_paren stm                   { } %prec IF
    | IF exp_paren bloc                  { } %prec IF
    | IF exp_paren stm ELSE stm          { }
    | IF exp_paren stm ELSE bloc         { }
    | IF exp_paren bloc ELSE stm         { }
    | IF exp_paren bloc ELSE bloc        { }

exp
    : exp_elem
    | exp_unary
    | exp_binary
    | exp_paren
    ;

exp_elem
    : ID                                 { }
    | NUM                                { }
    | REAL                               { }
    | STRING                             { }
    ;

exp_unary
    : MINUS exp                          { } %prec UMINUS
    ;

exp_binary
    : exp_binary_bool
    | exp_binary_math
    ;

exp_binary_bool
    : exp LT exp                         { }
    | exp GT exp                         { }
    | exp DEQ exp                        { }
    | exp NEQ exp                        { }
    | exp LTEQ exp                       { }
    | exp GTEQ exp                       { }
    ;

exp_binary_math
    : exp PLUS exp                       { }
    | exp MINUS exp                      { }
    | exp TIMES exp                      { }
    | exp DIV exp                        { }
    ;

exp_paren
    : LPAREN exp RPAREN                  { }
    ;
%%
