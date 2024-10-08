%{
#include "ast.h"
#include "lexer.h"
#include "tiger.y.h"
%}

id                    [a-zA-Z_][a-zA-Z0-9_]*
real                  [0-9]+\.[0-9]+
str                   \"[^"\n]*\"
int                   [0-9]+

%%
if                    { ADJ (); RET (IF     ); }
var                   { ADJ (); RET (VAR    ); }
func                  { ADJ (); RET (FUNC   ); }
else                  { ADJ (); RET (ELSE   ); }
type                  { ADJ (); RET (TYPE   ); }
while                 { ADJ (); RET (WHILE  ); }
union                 { ADJ (); RET (UNION  ); }
struct                { ADJ (); RET (STRUCT ); }
return                { ADJ (); RET (RETURN ); }

{id}                  { ADJ (); RET (ID     ); }
{str}                 { ADJ (); RET (STR    ); }
{int}                 { ADJ (); RET (INT    ); }
{real}                { ADJ (); RET (REAL   ); }

"="                   { ADJ (); RET (EQ     ); }
"+"                   { ADJ (); RET (PLUS   ); }
"-"                   { ADJ (); RET (MINUS  ); }
"*"                   { ADJ (); RET (TIMES  ); }
"/"                   { ADJ (); RET (DIV    ); }
"%"                   { ADJ (); RET (MOD    ); }
"&"                   { ADJ (); RET (AND    ); }
"^"                   { ADJ (); RET (XOR    ); }
"|"                   { ADJ (); RET (OR     ); }

"("                   { ADJ (); RET (LPAREN ); }
")"                   { ADJ (); RET (RPAREN ); }
"["                   { ADJ (); RET (LBRACK ); }
"]"                   { ADJ (); RET (RBRACK ); }
"{"                   { ADJ (); RET (LBRACE ); }
"}"                   { ADJ (); RET (RBRACE ); }

"."                   { ADJ (); RET (DOT    ); }
","                   { ADJ (); RET (COMMA  ); }
";"                   { ADJ (); RET (SEMI   ); }
":"                   { ADJ (); RET (COLON  ); }

"->"                  { ADJ (); RET (PMEM   ); }
"=="                  { ADJ (); RET (LEQ    ); }
"!="                  { ADJ (); RET (NEQ    ); }
"<="                  { ADJ (); RET (LTEQ   ); }
">="                  { ADJ (); RET (GTEQ   ); }
"&&"                  { ADJ (); RET (LAND   ); }
"||"                  { ADJ (); RET (LOR    ); }

[ \t]+                { ADJ ();                }
"//".*                { ADJ ();                }
\n                    { NLN ();                }
.                     { OTR ();                }
%%

int
yywrap (void)
{
  return 1;
}
