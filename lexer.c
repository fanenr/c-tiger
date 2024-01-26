#include "lexer.h"
#include "tiger.y.h"

extern int yyleng;
extern const char *yytext;

ast_pos m_pos = { .ln = 1, .ch = 1 };

void
adjust (void)
{
  m_pos.ch += yyleng;
}

void
nline (void)
{
  m_pos.ln++;
  m_pos.ch = 1;
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
  yylval.tok.pos.ch = m_pos.ch - yyleng;
  yylval.tok.pos.ln = m_pos.ln;
  yylval.tok.kind = tok;
  switch (tok)
    {
    case NUM:
      yylval.tok.num = checked_atol (yytext);
      break;
    case REAL:
      yylval.tok.real = checked_atod (yytext);
      break;
    case ID:
    case STR:
      yylval.tok.str = checked_strdup (yytext);
      break;
    }
  return tok;
}

int
yywrap (void)
{
  return 1;
}
