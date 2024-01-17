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

#endif
