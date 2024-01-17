%{
#include "token.h"
extern void nline (void);
extern void other (void);
%}

%%
if return IF;
for return FOR;

[a-zA-Z_][a-zA-Z0-9_]* return ID;
\"[^"\n]*\" return STRING;
[0-9]+ return NUM;

"=" return EQ;
"+" return PLUS;
"-" return MINUS;
"*" return TIMES;
"/" return DIV;
"," return COMMA;
";" return SEMI;

[ \t]+ /* ignore */
\n { nline (); }

. { other (); };
%%
