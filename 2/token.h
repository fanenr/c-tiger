#ifndef TOKENS_H
#define TOKENS_H

typedef struct token
{
  int kind;
  union
  {
    char *id;
    long num;
    char *string;
  } data;
} token;

enum
{
  ID,
  IF,
  NUM,
  FOR,
  COMMA,
  STRING,
  ILLEGAL,
};

#endif
