%{
#include "ast.h"
#include "lexer.h"
#include "tiger.y.h"
%}

id                    [a-zA-Z_][a-zA-Z0-9_]*
real                  [0-9]+\.[0-9]+
str                   \"[^"\n]*\"
num                   [0-9]+

%%
if                    { ADJ (); RET (IF     ); }
var                   { ADJ (); RET (VAR    ); }
func                  { ADJ (); RET (FUNC   ); }
else                  { ADJ (); RET (ELSE   ); }
type                  { ADJ (); RET (TYPE   ); }
while                 { ADJ (); RET (WHILE  ); }

{id}                  { ADJ (); RET (ID     ); }
{num}                 { ADJ (); RET (NUM    ); }
{str}                 { ADJ (); RET (STR    ); }
{real}                { ADJ (); RET (REAL   ); }
                                                
"="                   { ADJ (); RET (EQ     ); }
"+"                   { ADJ (); RET (PLUS   ); }
"-"                   { ADJ (); RET (MINUS  ); }
"*"                   { ADJ (); RET (TIMES  ); }
"/"                   { ADJ (); RET (DIV    ); }
                                                
"("                   { ADJ (); RET (LPAREN ); }
")"                   { ADJ (); RET (RPAREN ); }
"{"                   { ADJ (); RET (LBRACE ); }
"}"                   { ADJ (); RET (RBRACE ); }

","                   { ADJ (); RET (COMMA  ); }
";"                   { ADJ (); RET (SEMI   ); }
":"                   { ADJ (); RET (COLON  ); }

"=="                  { ADJ (); RET (LEQ    ); }
"!="                  { ADJ (); RET (NEQ    ); }
"<="                  { ADJ (); RET (LTEQ   ); }
">="                  { ADJ (); RET (GTEQ   ); }
                                                
[ \t]+                { ADJ ();                }
#.*                   { ADJ ();                }
\n                    { NLN ();                }
.                     { OTR ();                }
%%
