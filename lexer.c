#include "ast.h"
#include "util.h"
#include "lexer.h"
#include "tiger.y.h"
#include <stdlib.h>

/* from flex */
extern int yyleng;
extern const char *yytext;

ast_pos m_pos = { .ln = 1, .ch = 0 };

void
adjust (void)
{
  m_pos.ch += yyleng;
}

void
nline (void)
{
  m_pos.ln++;
  m_pos.ch = 0;
}

void
other (void)
{
  error ("error occured at %u:%u: use unkonwn type token %s\n", m_pos.ln,
         m_pos.ch, yytext);
}

int
handle (int tok)
{
  switch (tok)
    {
    case ID:
    case STR:
      yylval.ptr = checked_strdup (yytext);
      break;

    case NUM:
      yylval.num = atoi (yytext);
      break;

    case REAL:
      yylval.real = atof (yytext);
      break;

    default:
      yylval.pos = m_pos;
      break;
    }
  return tok;
}

int
yywrap (void)
{
  return 1;
}