%{
#include "token.h"
%}

%%
if return IF;
for return FOR;
[a-zA-Z_][a-zA-Z0-9_]+ return ID;
\"[^"\n]*\" return STRING;
[0-9]+ return NUM;
\, return COMMA;
\n return NLINE;
[ \t\r]+ return WSPACE;
. return OTHER;
%%
