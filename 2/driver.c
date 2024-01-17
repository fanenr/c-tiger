#include "token.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

extern int yylex (void);

extern int yyleng;
extern const char *yytext;

const char *const tokname[] = {
  [ID] = "ID",   [IF] = "IF",       [NUM] = "NUM",
  [FOR] = "FOR", [COMMA] = "COMMA", [STRING] = "STRING",
};

/* token list */
static token *token_list;
static size_t token_cap;
static size_t token_size;

static void token_add (int type);

int
main (int argc, char **argv)
{
  if (argc != 2)
    {
      fprintf (stderr, "usage: %s *.tig\n", argv[0]);
      exit (1);
    }

  reset (argv[1]);

  for (int tok = yylex (); tok; tok = yylex ())
    switch (tok)
      {
      case NLINE: /* new line */
        linepos++;
        chpos = 1;
        break;
      case WSPACE: /* white space */
        chpos += yyleng;
        break;
      default:
        token_add (tok);
        break;
      }

  for (size_t i = 0; i < token_size; i++)
    {
      token *tok = token_list + i;
      printf ("token: %s", tokname[tok->kind]);
      switch (tok->kind)
        {
        case NUM:
          printf (" (%ld)", tok->data.num);
          break;

        case ID:
        case STRING:
          printf (" (%s)", tok->data.str);
          break;

        default:
          break;
        }
      printf ("\n");
    }
}

void
token_add (int type)
{
  if (type <= TOK_ST || type >= TOK_ED)
    error ("unknown type token `%s`\n", yytext);

  if (token_size + 1 > token_cap)
    {
      size_t cap = token_cap == 0 ? 8 : 2 * token_cap;
      token *new = realloc (token_list, cap * sizeof (token));
      if (new == NULL)
        {
          fprintf (stderr, "memory allocate failed\n");
          exit (1);
        }
      token_cap = cap;
      token_list = new;
    }

  token tok = { .kind = type };
  switch (type)
    {
    case ID:
    case STRING:
      tok.data.str = string (yytext);
      break;
    case NUM:
      tok.data.num = atol (yytext);
      break;
    default:
      break;
    }

  token_list[token_size++] = tok;
  chpos += yyleng;
}

int
yywrap (void)
{
  return 1;
}
