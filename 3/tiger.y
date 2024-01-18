%{
#include <stdio.h>

extern int yylex (void);
extern void yyerror(const char *);
%}

%start prog

%token IF ELSE WHILE
%token ID NUM STRING
%token EQ PLUS MINUS TIMES DIV
%token COMMA SEMI
%token LPAREN RPAREN LBRACE RBRACE

%nonassoc IF
%nonassoc ELSE
%nonassoc EQ
%left COMMA
%left PLUS MINUS
%left TIMES DIV

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
    : stm SEMI                           { }
    | stms stm SEMI                      { }
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
    : IF exp_paren stm                   { }
    | IF exp_paren bloc                  { }
    | IF exp_paren stm ELSE stm          { }
    | IF exp_paren stm ELSE bloc         { }
    | IF exp_paren bloc ELSE stm         { }
    | IF exp_paren bloc ELSE bloc        { }

exp
    : exp_literal
    | exp_paren
    | exp_comma           
    | exp_math
    ;

exp_literal
    : ID                                 { }
    | NUM                                { }
    | STRING                             { }
    ;

exp_paren
    : LPAREN exp RPAREN                  { }
    ;

exp_comma
    : stm COMMA exp                      { }
    ;

exp_math
    : MINUS exp                          { }
    | exp DIV exp                        { }
    | exp PLUS exp                       { }
    | exp MINUS exp                      { }
    | exp TIMES exp                      { }
    ;
%%
