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

%token <real> REAL
%token <num>  NUM
%token <ptr>  ID STR
%token <pos>
       IF ELSE TYPE WHILE VAR FUNC
       EQ PLUS MINUS TIMES DIV LT GT LEQ NEQ LTEQ GTEQ
       LPAREN RPAREN LBRACK RBRACK LBRACE RBRACE
       COMMA SEMI COLON

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
      bloc_elem type
      def def_var def_type def_func
      stm stm_assign stm_while stm_if
      exp exp_elem exp_paren exp_unary exp_binary
      exp_binary_math exp_binary_logic

%%
prog
    : def
      { }
    | prog def
      { }
    ;

bloc
    : bloc_elem
      { AST_BLOC_NEW ($1); }
    | bloc bloc_elem
      { }
    ;

bloc_elem
    : def
      { $$ = $1; }
    | stm
      { $$ = $1; }
    ;

type
    : ID
      { }
    | type ID
      { }
    | LBRACK RBRACK type
      { }
    | TIMES type
      { }
    ;

def
    : def_var
      { $$ = $1; }
    | def_type
      { $$ = $1; }
    | def_func
      { $$ = $1; }
    ;

def_var
    : VAR ID COLON type SEMI
      { $$ = AST_DEF_NEW (VAR, $3, $1, $2, 0);  }
    | VAR ID COLON type EQ exp SEMI
      { $$ = AST_DEF_NEW (VAR, $5, $1, $2, $4); }
    ;

def_type
    : TYPE ID EQ type SEMI
      { $$ = AST_DEF_NEW (TYPE, $1, $2, $4);    }
    ;

def_func
    : FUNC ID LPAREN RPAREN type LBRACE RBRACE
      { $$ = AST_DEF_NEW (FUNC, $3, $1, $2, 0, 0); }
    | FUNC ID LPAREN RPAREN type LBRACE bloc RBRACE
      { $$ = AST_DEF_NEW (FUNC, $3, $1, $2, 0, 1); }
    | FUNC ID LPAREN parm RPAREN type LBRACE RBRACE
      { $$ = AST_DEF_NEW (FUNC, $3, $1, $2, 1, 0); }
    | FUNC ID LPAREN parm RPAREN type LBRACE bloc RBRACE
      { $$ = AST_DEF_NEW (FUNC, $3, $1, $2, 1, 1); }
    ;

parm
    : ID COLON type
      { ast_func_parm_push (pos, $1); }
    | parm COMMA ID COLON type
      { ast_func_parm_push (pos, $3); }
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
    : ID EQ exp SEMI
      { $$ = AST_STM_NEW (ASSIGN, $2, $1, $3);  }
    ;

stm_while
    : WHILE LPAREN exp RPAREN LBRACE bloc RBRACE
      { $$ = AST_STM_NEW (WHILE, $1, $2, 1);    }
    ;

stm_if
    : IF LPAREN exp RPAREN LBRACE bloc RBRACE
      { $$ = AST_STM_NEW (IF1, $1, $2, 1, 0);   }
    | IF LPAREN exp RPAREN LBRACE bloc RBRACE ELSE LBRACE bloc RBRACE
      { $$ = AST_STM_NEW (IF2, $1, $2, 1, 1);   }
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
    | STR
      { $$ = AST_EXP_NEW (ELEM_STR, pos, $1);    }
    | REAL
      { $$ = AST_EXP_NEW (ELEM_REAL, pos, $1);   }
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
      { $$ = AST_EXP_NEW (BINARY_PLUS, $2, $1, $3);  }
    | exp MINUS exp
      { $$ = AST_EXP_NEW (BINARY_MINUS, $2, $1, $3); }
    | exp TIMES exp
      { $$ = AST_EXP_NEW (BINARY_TIMES, $2, $1, $3); }
    | exp DIV exp
      { $$ = AST_EXP_NEW (BINARY_DIV, $2, $1, $3);   }
    ;

exp_binary_logic
    : exp LT exp
      { $$ = AST_EXP_NEW (BINARY_LT, $2, $1, $3);    }
    | exp GT exp
      { $$ = AST_EXP_NEW (BINARY_GT, $2, $1, $3);    }
    | exp LEQ exp
      { $$ = AST_EXP_NEW (BINARY_LEQ, $2, $1, $3);   }
    | exp NEQ exp
      { $$ = AST_EXP_NEW (BINARY_NEQ, $2, $1, $3);   }
    | exp LTEQ exp
      { $$ = AST_EXP_NEW (BINARY_LTEQ, $2, $1, $3);  }
    | exp GTEQ exp
      { $$ = AST_EXP_NEW (BINARY_GTEQ, $2, $1, $3);  }
    ;
%%
