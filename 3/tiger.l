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

digit                 [0-9]
alpha                 [a-zA-Z_]
str                   \"[^"\n]*\"
id                    {alpha}[{alpha}{digit}]*

%%
if                    { ADJ (); RET (IF     ); }
else                  { ADJ (); RET (ELSE   ); }
while                 { ADJ (); RET (WHILE  ); }
                                                    
{id}                  { ADJ (); RET (ID     ); }
{str}                 { ADJ (); RET (STRING ); }
{digit}+              { ADJ (); RET (NUM    ); }
                                                
"="                   { ADJ (); RET (EQ     ); }
"+"                   { ADJ (); RET (PLUS   ); }
"-"                   { ADJ (); RET (MINUS  ); }
"*"                   { ADJ (); RET (TIMES  ); }
"/"                   { ADJ (); RET (DIV    ); }
","                   { ADJ (); RET (COMMA  ); }
";"                   { ADJ (); RET (SEMI   ); }
                                                
[ \t]+                { ADJ ();                }
\n                    { NLN ();                }
.                     { OTR ();                }
%%
