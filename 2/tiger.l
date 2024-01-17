%{
#include "util.h"
#include "token.h"

extern token token_build (int type, const char *text);
extern void token_add (token tok);

#define ADD(type) token_add (token_build (type, yytext))
#define UPLINEPOS() linepos++; chpos = 1
#define UPCHPOS() chpos += yyleng
%}

%%
if { ADD(IF); UPCHPOS(); }
for { ADD(FOR); UPCHPOS(); }
[a-zA-Z_][a-zA-Z0-9_]+ { ADD(ID); UPCHPOS(); }
\"[^"\n]*\" { ADD(STRING); UPCHPOS(); }
[0-9]+ { ADD(NUM); UPCHPOS(); }
\, { ADD(COMMA); UPCHPOS(); }
\n { UPLINEPOS(); }
[ \t\r]+ { /* skip */ }
. { ADD(-1); /* illegal */ }
%%

int
yywrap (void)
{
  return 1;
}
