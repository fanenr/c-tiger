%{
#include <stdio.h>

extern int yylex (void);
extern void yyerror(const char *);
%}

%start prog
%token IF FOR 
%token ID NUM STRING
%token EQ PLUS MINUS TIMES DIV
%token COMMA SEMI COLON
%token WSPACE NLINE OTHER

%left PLUS MINUS
%left TIMES DIV

%union {
         long num;
         char *str;
         double real;
       }

%%
prog
    : stms                  {
                            }
    ;

stms
    : stm                   {
                            }

    | stms SEMI stm         {
                            }
    ;

stm
    : ID EQ exp             {
                              printf ("assign ");
                            }
    ;

exp
    : ID                    {
                            }

    | NUM                   {
                            }

    | exp PLUS exp          {
                              printf ("plus ");
                            }

    | exp MINUS exp         {
                            }

    | exp TIMES exp         {
                              printf ("times ");
                            }

    | exp DIV exp           {
                            }
    ;
%%
