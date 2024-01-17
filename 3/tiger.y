%{
#include <stdio.h>
extern int yylex (void);
extern void yyerror(const char *);
%}

%token ST 259
%token IF FOR ID NUM STRING
%token EQ PLUS MINUS TIMES DIV COMMA SEMI
%token OTHER NLINE WSPACE
%token ED

%start prog
%left PLUS MINUS
%left TIMES DIV

%%
prog: stms

stms: stm
    | stms SEMI stm
    ;

stm: ID EQ exp
    ;

exp: NUM
    | exp PLUS exp
    | exp MINUS exp
    | exp TIMES exp
    | exp DIV exp
    ;
%%
