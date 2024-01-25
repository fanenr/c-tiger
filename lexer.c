#include "ast.h"
#include "util.h"
#include "lexer.h"
#include "tiger.y.h"
#include <stdlib.h>

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
  switch (tok)
    {
    case ID:
    case STR:
      yylval.val.pos = (ast_pos){ .ln = m_pos.ln, .ch = m_pos.ch - yyleng };
      yylval.val.ptr = checked_strdup (yytext);
      break;

    case NUM:
      yylval.val.pos = (ast_pos){ .ln = m_pos.ln, .ch = m_pos.ch - yyleng };
      yylval.val.num = atoi (yytext);
      break;

    case REAL:
      yylval.val.pos = (ast_pos){ .ln = m_pos.ln, .ch = m_pos.ch - yyleng };
      yylval.val.real = atof (yytext);
      break;

    default:
      yylval.tok.pos = (ast_pos){ .ln = m_pos.ln, .ch = m_pos.ch - yyleng };
      yylval.tok.kind = tok;
      break;
    }
  return tok;
}

int
yywrap (void)
{
  return 1;
}