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
  TOK_ST,
  IF,     /* if */
  FOR,    /* for */
  ID,     /* id */
  NUM,    /* num */
  STRING, /* ".*" */
  COMMA,  /* , */
  TOK_ED,

  OTHER,  /* . */
  NLINE,  /* \n */
  WSPACE, /* [ \t\r]+ */
};

#endif
