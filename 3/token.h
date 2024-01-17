#ifndef TOKENS_H
#define TOKENS_H

typedef struct token
{
  int kind;
  union
  {
    long num;
    char *str;
  } data;
} token;

enum
{
  ST = 260,
  IF,
  FOR,
  ID,
  NUM,
  STRING,
  EQ,
  PLUS,
  MINUS,
  TIMES,
  DIV,
  COMMA,
  SEMI,
  OTHER,
  NLINE,
  WSPACE,
  ED,
};

#endif
