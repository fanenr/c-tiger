%{
#include "tiger.y.h"
extern void nline (void);
extern void other (void);
%}

id                        [a-zA-Z_][a-zA-Z0-9_]*
str                       \"[^"\n]*\"

%%
if                        { return IF;     }
for                       { return FOR;    }
                                            
{id}                      { return ID;     }
{str}                     { return STRING; }
[0-9]+                    { return NUM;    }
                                            
"="                       { return EQ;     }
"+"                       { return PLUS;   }
"-"                       { return MINUS;  }
"*"                       { return TIMES;  }
"/"                       { return DIV;    }
","                       { return COMMA;  }
":"                       { return COLON;  }
";"                       { return SEMI;   }
                                            
[ \t]+                    {                }
\n                        { nline ();      }
.                         { other ();      }
%%
