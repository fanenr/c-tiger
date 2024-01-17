%{
extern int yylex (void);
extern void yyerror(const char *);
%}

%start stms
%token ST 259
%token IF FOR ID NUM STRING
%token EQ PLUS MINUS TIMES DIV COMMA 
%token OTHER NLINE WSPACE

%left PLUS MINUS
%left TIMES DIV

%%
stms: stm
    | stm stms
    ;

stm: exp

exp: NUM
    | exp PLUS exp
    | exp MINUS exp
    | exp TIMES exp
    | exp DIV exp
    ;
%%
