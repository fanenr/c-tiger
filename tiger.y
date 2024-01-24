%{
#include "lexer.h"
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

%token <real> REAL
%token <num>  NUM
%token <ptr>  ID STR
%token <pos>
       IF VAR ELSE TYPE FUNC WHILE STRUCT UNION
       EQ PLUS MINUS TIMES DIV MOD LT GT LEQ NEQ LTEQ GTEQ
       OR AND XOR LOR DOT PMEM LAND
       LPAREN RPAREN LBRACK RBRACK LBRACE RBRACE
       COMMA SEMI COLON

%nonassoc IF
%nonassoc ELSE

%right EQ
%left LOR
%left LAND
%left OR
%left XOR
%left AND
%left LEQ NEQ
%left LT GT LTEQ GTEQ
%left PLUS MINUS
%left TIMES DIV MOD
%right ADDR
%right DADDR
%right UPLUS UMINUS
%left PMEM
%left DOT

%start prog
%type <ptr>
      bloc_elem
      def def_var def_type def_func
      stm stm_assign stm_while stm_if
      exp exp_elem exp_paren exp_unary exp_binary
      exp_binary_bit exp_binary_math exp_binary_logic

%%
prog
    : def
      { }
    | prog def
      { }
    ;

bloc
    : bloc_elem
      { GENV_PUSH (1, $1); }
    | bloc bloc_elem
      { GENV_PUSH (2, $2); }
    ;

bloc_elem
    : def
      { $$ = $1; }
    | stm
      { $$ = $1; }
    ;

type
    : ID
      { GTYPE_PUSH (1, $1); }
    | TIMES type
      { GTYPE_PUSH (2, 0);  }
    | LBRACK RBRACK type
      { GTYPE_PUSH (3, 0);  }
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
      { $$ = AST_DEF_NEW (VAR, $1, $2, GTYPE, 0);  }
    | VAR ID COLON type EQ exp SEMI
      { $$ = AST_DEF_NEW (VAR, $1, $2, GTYPE, $6); }
    ;

def_type
    : TYPE ID EQ type SEMI
      { $$ = AST_DEF_NEW (TYPE, $1, 1, $2, GTYPE); }
    | UNION ID LBRACE parm COMMA RBRACE
      { $$ = AST_DEF_NEW (TYPE, $1, 2, $2, GPARM); }
    | STRUCT ID LBRACE parm COMMA RBRACE
      { $$ = AST_DEF_NEW (TYPE, $1, 3, $2, GPARM); }
    ;

def_func
    : FUNC ID LPAREN RPAREN type LBRACE RBRACE
      { $$ = AST_DEF_NEW (FUNC, $1, $2, 0, GTYPE, 0);        }
    | FUNC ID LPAREN RPAREN type LBRACE bloc RBRACE
      { $$ = AST_DEF_NEW (FUNC, $1, $2, 0, GTYPE, GENV);     }
    | FUNC ID LPAREN parm RPAREN type LBRACE RBRACE
      { $$ = AST_DEF_NEW (FUNC, $1, $2, GPARM, GTYPE, 0);    }
    | FUNC ID LPAREN parm RPAREN type LBRACE bloc RBRACE
      { $$ = AST_DEF_NEW (FUNC, $1, $2, GPARM, GTYPE, GENV); }
    ;

parm
    : ID COLON type
      { GPARM_PUSH (1, $1, GTYPE); }
    | parm COMMA ID COLON type
      { GPARM_PUSH (2, $3, GTYPE); }
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
      { $$ = AST_STM_NEW (IF1, $1, $2, GENV, 0); }
    | IF LPAREN exp RPAREN LBRACE bloc RBRACE ELSE LBRACE bloc RBRACE
      { $$ = AST_STM_NEW (IF2, $1, $2, 0, GENV); }
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
      { $$ = AST_EXP_NEW (ELEM_ID, GPOS, $1);      }
    | NUM
      { $$ = AST_EXP_NEW (ELEM_NUM, GPOS, $1);     }
    | STR
      { $$ = AST_EXP_NEW (ELEM_STR, GPOS, $1);     }
    | REAL
      { $$ = AST_EXP_NEW (ELEM_REAL, GPOS, $1);    }
    ;

exp_paren
    : LPAREN exp RPAREN
      { $$ = $2; }
    ;

exp_unary
    : PLUS exp %prec UPLUS
      { $$ = AST_EXP_NEW (UN_UMINUS, $1, $2); }
    | MINUS exp %prec UMINUS
      { $$ = AST_EXP_NEW (UN_UMINUS, $1, $2); }
    | TIMES exp %prec DADDR
      { }
    | AND exp %prec ADDR
      { }
    ;

exp_binary
    : exp_binary_bit
      { $$ = $1; }
    | exp_binary_mem
      { }
    | exp_binary_math
      { $$ = $1; }
    | exp_binary_logic
      { $$ = $1; }
    ;

exp_binary_bit
    : exp AND exp
      { }
    | exp OR exp
      { }
    | exp XOR exp
      { }
    ;

exp_binary_mem
    : exp DOT exp
      { }
    | exp PMEM exp
      { }
    ;

exp_binary_math
    : exp PLUS exp
      { $$ = AST_EXP_NEW (BIN_PLUS, $2, $1, $3);  }
    | exp MINUS exp
      { $$ = AST_EXP_NEW (BIN_MINUS, $2, $1, $3); }
    | exp TIMES exp
      { $$ = AST_EXP_NEW (BIN_TIMES, $2, $1, $3); }
    | exp DIV exp
      { $$ = AST_EXP_NEW (BIN_DIV, $2, $1, $3);   }
    | exp MOD exp
      { }
    ;

exp_binary_logic
    : exp LT exp
      { $$ = AST_EXP_NEW (BIN_LT, $2, $1, $3);    }
    | exp GT exp
      { $$ = AST_EXP_NEW (BIN_GT, $2, $1, $3);    }
    | exp LEQ exp
      { $$ = AST_EXP_NEW (BIN_LEQ, $2, $1, $3);   }
    | exp NEQ exp
      { $$ = AST_EXP_NEW (BIN_NEQ, $2, $1, $3);   }
    | exp LTEQ exp
      { $$ = AST_EXP_NEW (BIN_LTEQ, $2, $1, $3);  }
    | exp GTEQ exp
      { $$ = AST_EXP_NEW (BIN_GTEQ, $2, $1, $3);  }
    | exp LAND exp
      { }
    | exp LOR exp
      { }
    ;  
%%
