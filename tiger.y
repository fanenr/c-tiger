%{
#include "parser.h"

extern int yylex (void);
extern void yyerror(const char *msg);
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

%left COMMA
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
%right DREF
%right UPLUS UMINUS
%left PMEM
%left DOT
%left LBRACK
%left LPAREN

%type <ptr>
      stm def exp type bloc
      stm_return stm_assign stm_while stm_if
      def_var def_type def_func def_func_parm
      exp_elem exp_paren exp_unary exp_binary
      exp_call exp_call_args exp_unary_mem exp_unary_math
      exp_binary_mem exp_binary_bit exp_binary_math exp_binary_logic

%start prog

%%
prog
    : def
      { ast_env_push_def (&prog, $1); }
    | prog def
      { ast_env_push_def (&prog, $2); }
    ;

type
    : ID
      { $$ = ast_type_new (0, $1);  }
    | TIMES type
      { $$ = ast_type_new ($2, $1); }
    ;

bloc
    : def
      { $$ = ast_env_push_def (0, $1);  }
    | stm
      { $$ = ast_env_push_stm (0, $1);  }
    | bloc def
      { $$ = ast_env_push_def ($1, $2); }
    | bloc stm
      { $$ = ast_env_push_stm ($1, $2); }
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
      { POS ($1); $$ = ast_def_var_new ($2, $4);  }
    ;

def_type
    : TYPE ID EQ type SEMI
      { POS ($1); $$ = ast_def_type_new ($2, $4);        }
    | UNION ID LBRACE bloc RBRACE
      { POS ($1); $$ = ast_def_type_union_new ($2, $4);  }
    | STRUCT ID LBRACE bloc RBRACE
      { POS ($1); $$ = ast_def_type_struct_new ($2, $4); }
    ;

def_func
    : FUNC ID LPAREN RPAREN type LBRACE RBRACE
      { POS ($1); $$ = ast_def_func_new ($2, 0, $5, 0);   }
    | FUNC ID LPAREN RPAREN type LBRACE bloc RBRACE
      { POS ($1); $$ = ast_def_func_new ($2, 0, $5, $7);  }
    | FUNC ID LPAREN def_func_parm RPAREN type LBRACE RBRACE
      { POS ($1); $$ = ast_def_func_new ($2, $4, $6, 0);  }
    | FUNC ID LPAREN def_func_parm RPAREN type LBRACE bloc RBRACE
      { POS ($1); $$ = ast_def_func_new ($2, $4, $6, $8); }
    ;

def_func_parm
    : ID COLON type
      { $$ = ast_def_func_parm_new (0, $1, $3);  }
    | def_func_parm COMMA ID COLON type
      { $$ = ast_def_func_parm_new ($1, $3, $5); }
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

stm_return
    : RETURN SEMI
      { POS ($1); $$ = ast_stm_return_new (0);  }
    | RETURN exp SEMI
      { POS ($1); $$ = ast_stm_return_new ($2); }
    ;

stm_assign
    : exp EQ exp SEMI
      { $$ = ast_stm_assign_new ($1, $3);  }
    ;

stm_while
    : WHILE LPAREN exp RPAREN LBRACE bloc RBRACE
      { POS ($1); $$ = ast_stm_while_new ($3, $6);   }
    ;

stm_if
    : IF LPAREN exp RPAREN LBRACE bloc RBRACE
      { POS ($1); $$ = ast_stm_if_new ($3, $6, 0);   }
    | IF LPAREN exp RPAREN LBRACE bloc RBRACE ELSE LBRACE bloc RBRACE
      { POS ($1); $$ = ast_stm_if_new ($3, $6, $10); }
    ;

exp
    : exp_elem
      { $$ = $1; }
    | exp_call
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
      { $$ = ast_exp_elem_new ($1); }
    | NUM
      { $$ = ast_exp_elem_new ($1); }
    | STR
      { $$ = ast_exp_elem_new ($1); }
    | REAL
      { $$ = ast_exp_elem_new ($1); }
    ;


exp_call
    : ID LPAREN RPAREN
      { $$ = ast_exp_call_new ($1, 0);  }
    | ID LPAREN exp_call_args RPAREN
      { $$ = ast_exp_call_new ($1, $3); }
    ;

exp_call_args
    : exp
      { $$ = ast_exp_call_args_new (0, $1);  }
    | exp_call_args COMMA exp
      { $$ = ast_exp_call_args_new ($1, $3); }
    ;

exp_paren
    : LPAREN exp RPAREN
      { $$ = $2; }
    ;

exp_unary
    : exp_unary_mem
      { $$ = $1; }
    | exp_unary_math
      { $$ = $1; }
    ;

exp_unary_mem
    : AND exp %prec ADDR
      { POS ($1); $$ = UN_NEW (ADDR, $2);   }
    | TIMES exp %prec DREF
      { POS ($1); $$ = UN_NEW (DREF, $2);   }
    ;

exp_unary_math
    : PLUS exp %prec UPLUS
      { POS ($1); $$ = UN_NEW (UPLUS, $2);  }
    | MINUS exp %prec UMINUS
      { POS ($1); $$ = UN_NEW (UMINUS, $2); }
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
    ;

exp_binary_mem
    : exp DOT exp
      { $$ = BIN_NEW (DMEM, $1, $3);  }
    | exp PMEM exp
      { $$ = BIN_NEW (PMEM, $1, $3);  }
    | exp LBRACK exp RBRACK
      { $$ = BIN_NEW (INDEX, $1, $3); }
    ;

exp_binary_bit
    : exp OR exp
      { $$ = BIN_NEW (OR, $1, $3);    }
    | exp AND exp
      { $$ = BIN_NEW (AND, $1, $3);   }
    | exp XOR exp
      { $$ = BIN_NEW (XOR, $1, $3);   }
    ;

exp_binary_math
    : exp PLUS exp
      { $$ = BIN_NEW (PLUS, $1, $3);  }
    | exp MINUS exp
      { $$ = BIN_NEW (MINUS, $1, $3); }
    | exp TIMES exp
      { $$ = BIN_NEW (TIMES, $1, $3); }
    | exp DIV exp
      { $$ = BIN_NEW (DIV, $1, $3);   }
    | exp MOD exp
      { $$ = BIN_NEW (MOD, $1, $3);   }
    ;

exp_binary_logic
    : exp LT exp
      { $$ = BIN_NEW (LT, $1, $3);    }
    | exp GT exp
      { $$ = BIN_NEW (GT, $1, $3);    }
    | exp LEQ exp
      { $$ = BIN_NEW (LEQ, $1, $3);   }
    | exp NEQ exp
      { $$ = BIN_NEW (NEQ, $1, $3);   }
    | exp LTEQ exp
      { $$ = BIN_NEW (LTEQ, $1, $3);  }
    | exp GTEQ exp
      { $$ = BIN_NEW (GTEQ, $1, $3);  }
    | exp LAND exp
      { $$ = BIN_NEW (LAND, $1, $3);  }
    | exp LOR exp
      { $$ = BIN_NEW (LOR, $1, $3);   }
    ;
%%

void
yyerror (const char *msg)
{
  parser_other (msg);
}
