%{
#include "lexer.h"
#include "tiger.y.h"
%}

id                    [a-zA-Z_][a-zA-Z0-9_]*
num                   [0-9]+
real                  [0-9]+\.[0-9]+
string                \"[^"\n]*\"

%%
if                    { ADJ (); RET (IF     ); }
else                  { ADJ (); RET (ELSE   ); }
while                 { ADJ (); RET (WHILE  ); }
                                                    
{id}                  { ADJ (); RET (ID     ); }
{num}                 { ADJ (); RET (NUM    ); }
{real}                { ADJ (); RET (REAL   ); }
{string}              { ADJ (); RET (STRING ); }
                                                
"="                   { ADJ (); RET (EQ     ); }
"+"                   { ADJ (); RET (PLUS   ); }
"-"                   { ADJ (); RET (MINUS  ); }
"*"                   { ADJ (); RET (TIMES  ); }
"/"                   { ADJ (); RET (DIV    ); }

"("                   { ADJ (); RET (LPAREN ); }
")"                   { ADJ (); RET (RPAREN ); }
"{"                   { ADJ (); RET (LBRACE ); }
"}"                   { ADJ (); RET (RBRACE ); }

"=="                  { ADJ (); RET (LEQ    ); }
"!="                  { ADJ (); RET (NEQ    ); }
"<="                  { ADJ (); RET (LTEQ   ); }
">="                  { ADJ (); RET (GTEQ   ); }
                                                
[ \t]+                { ADJ ();                }
\n                    { NLN ();                }
.                     { OTR ();                }
%%
