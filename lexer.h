#ifndef LEXER_H
#define LEXER_H

#include "ast.h"

extern ast_pos m_pos;

extern void nline (void);
extern void other (void);
extern void adjust (void);
extern int handle (int tok);

#define NLN() nline ()
#define OTR() other ()
#define ADJ() adjust ()
#define RET(TOK) return handle (TOK)

#endif