#include "token.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

extern int yylex (void);

const char *const tokname[] = {
  [ID] = "ID",   [IF] = "IF",       [NUM] = "NUM",
  [FOR] = "FOR", [COMMA] = "COMMA", [STRING] = "STRING",
};

token *token_list;
static size_t token_cap;
static size_t token_size;

int
main (int argc, char **argv)
{
  if (argc != 2)
    {
      fprintf (stderr, "usage: %s *.tig\n", argv[0]);
      exit (1);
    }

  reset (argv[1]);
  yylex ();

  token *tok;
  for (size_t i = 0; i < token_size; i++)
    {
      tok = token_list + i;
      switch (tok->kind)
        {
        case IF:
        case FOR:
        case COMMA:
          printf ("token: %s\n", tokname[tok->kind]);
          break;

        case NUM:
          printf ("token: %s (%ld)\n", tokname[tok->kind], tok->data.num);
          break;

        case ID:
        case STRING:
          printf ("token: %s (%s)\n", tokname[tok->kind], tok->data.id);
          break;
        }
    }
}

void
token_add (token tok)
{
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
  token_list[token_size++] = tok;
}

token
token_build (int type, const char *text)
{
  if (type < 0 || type >= ILLEGAL)
    error ("unknown type token: %s\n", text);

  token tok;
  tok.kind = type;
  switch (type)
    {
    case NUM:
      tok.data.num = atol (text);
      break;

    case ID:
      tok.data.id = string (text);

    case STRING:
      tok.data.string = string (text);

    default:
      break;
    }
  return tok;
}
