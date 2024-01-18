%{
#include "tiger.y.h"

extern void nline (void);
extern void other (void);
extern void adjust (void);

#define NLN() nline ()
#define OTR() other ()
#define ADJ() adjust ()
#define RET(TOK) return TOK
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

"=="                  { ADJ (); RET (DEQ    ); }
"!="                  { ADJ (); RET (NEQ    ); }
"<="                  { ADJ (); RET (LTEQ   ); }
">="                  { ADJ (); RET (GTEQ   ); }
                                                
[ \t]+                { ADJ ();                }
\n                    { NLN ();                }
.                     { OTR ();                }
%%
