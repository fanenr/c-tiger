#include "lexer.h"
#include "ast.h"
#include "common.h"

#include "tiger.y.h"
#include <stdlib.h>

long conv_atol (const char *src);
double conv_atod (const char *src);

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
  ast_error ("use unkonw token %s", m_pos, yytext);
}

int
handle (int tok)
{
  yylval.tok.pos.ch = m_pos.ch - yyleng;
  yylval.tok.pos.ln = m_pos.ln;
  yylval.tok.kind = tok;

  switch (tok)
    {
    case REAL:
      yylval.tok.real = conv_atod (yytext);
      break;
    case NUM:
      yylval.tok.num = conv_atol (yytext);
      break;
    case STR:
    case ID:
      {
        mstr_t *str = &yylval.tok.str;
        mstr_assign_cstr (str, yytext);
      }
      break;
    }

  return tok;
}

long
conv_atol (const char *src)
{
  char *end;
  long ret = strtol (src, &end, 0);
  if (end == src)
    error ("can not convert to long");
  return ret;
}

double
conv_atod (const char *src)
{
  char *end;
  double ret = strtod (src, &end);
  if (end == src)
    error ("invalid string\n");
  return ret;
}
