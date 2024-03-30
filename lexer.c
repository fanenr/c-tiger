#include "lexer.h"
#include "ast.h"
#include "common.h"

#include "tiger.y.h"
#include <stdlib.h>

extern int yyleng;
extern const char *yytext;

long conv_atol (const char *src);
double conv_atod (const char *src);

static ast_pos m_pos = { .ln = 1, .ch = 1 };

void
lexer_chpos (void)
{
  m_pos.ch += yyleng;
}

void
lexer_nline (void)
{
  m_pos.ln++;
  m_pos.ch = 1;
}

void
lexer_other (void)
{
  ast_error (m_pos, "unkonw token %s", yytext);
}

int
lexer_ret (int kind)
{
  yylval.tok.pos.ch = m_pos.ch - yyleng;
  yylval.tok.pos.ln = m_pos.ln;
  yylval.tok.kind = kind;
  
  mstr_t *str = &yylval.tok.str;

  switch (kind)
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
        mstr_init (str);
        mstr_assign_cstr (str, yytext);
      }
      break;
    }

  return kind;
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
