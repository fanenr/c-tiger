%{
#include "lexer.h"
#include "parser.h"

extern int yylex (void);
extern void yyerror(const char *);
%}

%union {
         void *ptr;
         ast_tok tok;
       }

%token <tok> NUM
%token <tok> REAL
%token <tok> ID STR
%token <tok>
       IF VAR ELSE TYPE FUNC WHILE UNION STRUCT RETURN
       EQ PLUS MINUS TIMES DIV MOD LT GT LEQ NEQ LTEQ GTEQ
       OR AND XOR LOR DOT PMEM LAND COMMA SEMI COLON
       LPAREN RPAREN LBRACK RBRACK LBRACE RBRACE

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
%left LBRACK
%left LPAREN

%type <ptr>
      prog bloc type
      def def_var def_type def_func def_func_parm
      stm stm_assign stm_return stm_while stm_if
      exp exp_elem exp_paren exp_unary exp_binary
      exp_binary_bit exp_binary_mem exp_binary_math
      exp_binary_logic exp_binary_call exp_comma

%start prog

%%
prog
    : def
      { $$ = ast_env_push (&prog, $1); }
    | prog def
      { $$ = ast_env_push (&prog, $1); }
    ;

bloc
    : def
      { $$ = ast_env_push (0, $1);  }
    | stm
      { $$ = ast_env_push (0, $1);  }
    | bloc def
      { $$ = ast_env_push ($1, $2); }
    | bloc stm
      { $$ = ast_env_push ($1, $2); }
    ;

type
    : ID
      { $$ = ast_type_push ($1, 0);  }
    | TIMES type
      { $$ = ast_type_push ($1, $2); }
    | LBRACK RBRACK type
      { $$ = ast_type_push ($1, $3); }
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
      { $$ = AST_DEF_NEW (VAR, $1, $2, $4, 0);  }
    | VAR ID COLON type EQ exp SEMI
      { $$ = AST_DEF_NEW (VAR, $1, $2, $4, $6); }
    ;

def_type
    : TYPE ID EQ type SEMI
      { $$ = AST_DEF_NEW (TYPE, $1, $2, $4); }
    | UNION ID LBRACE bloc RBRACE
      { $$ = AST_DEF_NEW (TYPE, $1, $2, $4); }
    | STRUCT ID LBRACE bloc RBRACE
      { $$ = AST_DEF_NEW (TYPE, $1, $2, $4); }
    ;

def_func
    : FUNC ID LPAREN RPAREN type LBRACE RBRACE
      { $$ = AST_DEF_NEW (FUNC, $1, $2, 0, $5, 0);   }
    | FUNC ID LPAREN RPAREN type LBRACE bloc RBRACE
      { $$ = AST_DEF_NEW (FUNC, $1, $2, 0, $5, $7);  }
    | FUNC ID LPAREN def_func_parm RPAREN type LBRACE RBRACE
      { $$ = AST_DEF_NEW (FUNC, $1, $2, $4, $6, 0);  }
    | FUNC ID LPAREN def_func_parm RPAREN type LBRACE bloc RBRACE
      { $$ = AST_DEF_NEW (FUNC, $1, $2, $4, $6, $8); }
    ;

def_func_parm
    : ID COLON type
      { $$ = ast_parm_push (0, $1, $3);  }
    | def_func_parm COMMA ID COLON type
      { $$ = ast_parm_push ($1, $3, $5); }
    ;

stm
    : stm_assign
      { $$ = $1; }
    | stm_return
      { $$ = $1; }
    | stm_while
      { $$ = $1; }
    | stm_if
      { $$ = $1; }
    ;

stm_assign
    : exp EQ exp SEMI
      { $$ = AST_STM_NEW (ASSIGN, $1, $3); }
    ;

stm_return
    : RETURN SEMI
      { $$ = AST_STM_NEW (RETURN, $1, 0);  }
    | RETURN exp SEMI
      { $$ = AST_STM_NEW (RETURN, $1, $2); }
    ;

stm_while
    : WHILE LPAREN exp RPAREN LBRACE bloc RBRACE
      { $$ = AST_STM_NEW (WHILE, $1, $3, $6);    }
    ;

stm_if
    : IF LPAREN exp RPAREN LBRACE bloc RBRACE
      { $$ = AST_STM_NEW (IF1, $1, $3, $6, 0);   }
    | IF LPAREN exp RPAREN LBRACE bloc RBRACE ELSE LBRACE bloc RBRACE
      { $$ = AST_STM_NEW (IF2, $1, $3, $6, $10); }
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
      { $$ = AST_EXP_NEW (ELEM_ID, $1);   }
    | NUM
      { $$ = AST_EXP_NEW (ELEM_NUM, $1);  }
    | STR
      { $$ = AST_EXP_NEW (ELEM_STR, $1);  }
    | REAL
      { $$ = AST_EXP_NEW (ELEM_REAL, $1); }
    ;

exp_paren
    : LPAREN exp RPAREN
      { $$ = $2; }
    ;

exp_unary
    : PLUS exp %prec UPLUS
      { $$ = AST_EXP_NEW (UN_UPLUS, $1, $2);  }
    | MINUS exp %prec UMINUS
      { $$ = AST_EXP_NEW (UN_UMINUS, $1, $2); }
    | TIMES exp %prec DADDR
      { $$ = AST_EXP_NEW (UN_DADDR, $1, $2);  }
    | AND exp %prec ADDR
      { $$ = AST_EXP_NEW (UN_ADDR, $1, $2);   }
    ;

exp_binary
    : exp_binary_bit
      { $$ = $1; }
    | exp_binary_mem
      { $$ = $1; }
    | exp_binary_math
      { $$ = $1; }
    | exp_binary_logic
      { $$ = $1; }
    | exp_binary_call
      { $$ = $1; }
    ;

exp_binary_bit
    : exp AND exp
      { $$ = AST_EXP_NEW (BIN_AND, $1, $3);   }
    | exp OR exp
      { $$ = AST_EXP_NEW (BIN_OR, $1, $3);    }
    | exp XOR exp
      { $$ = AST_EXP_NEW (BIN_XOR, $1, $3);   }
    ;

exp_binary_mem
    : exp DOT exp
      { $$ = AST_EXP_NEW (BIN_MEM, $1, $3);   }
    | exp PMEM exp
      { $$ = AST_EXP_NEW (BIN_PMEM, $1, $3);  }
    | exp LBRACK exp RBRACK
      { $$ = AST_EXP_NEW (BIN_INDEX, $1, $3); }
    ;

exp_binary_math
    : exp PLUS exp
      { $$ = AST_EXP_NEW (BIN_PLUS, $1, $3);  }
    | exp MINUS exp
      { $$ = AST_EXP_NEW (BIN_MINUS, $1, $3); }
    | exp TIMES exp
      { $$ = AST_EXP_NEW (BIN_TIMES, $1, $3); }
    | exp DIV exp
      { $$ = AST_EXP_NEW (BIN_DIV, $1, $3);   }
    | exp MOD exp
      { $$ = AST_EXP_NEW (BIN_MOD, $1, $3);   }
    ;

exp_binary_logic
    : exp LT exp
      { $$ = AST_EXP_NEW (BIN_LT, $1, $3);    }
    | exp GT exp
      { $$ = AST_EXP_NEW (BIN_GT, $1, $3);    }
    | exp LEQ exp
      { $$ = AST_EXP_NEW (BIN_LEQ, $1, $3);   }
    | exp NEQ exp
      { $$ = AST_EXP_NEW (BIN_NEQ, $1, $3);   }
    | exp LTEQ exp
      { $$ = AST_EXP_NEW (BIN_LTEQ, $1, $3);  }
    | exp GTEQ exp
      { $$ = AST_EXP_NEW (BIN_GTEQ, $1, $3);  }
    | exp LAND exp
      { $$ = AST_EXP_NEW (BIN_LAND, $1, $3);  }
    | exp LOR exp
      { $$ = AST_EXP_NEW (BIN_LOR, $1, $3);   }
    ;

exp_binary_call
    : exp LPAREN RPAREN
      { $$ = AST_EXP_NEW (BIN_CALL, $1, $3);  }
    | exp LPAREN exp_comma RPAREN
      { $$ = AST_EXP_NEW (BIN_CALL, $1, $3);  }
    ;

exp_comma
    : exp
      { $$ = AST_EXP_NEW (COMMA, 0, $1);      }
    | exp_comma COMMA exp
      { $$ = AST_EXP_NEW (COMMA, $1, $3);     }
    ;
%%
