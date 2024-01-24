#ifndef LEXER_H
#define LEXER_H

#include "ast.h"

/* from lexer */
extern ast_pos m_pos;
#define GPOS m_pos

extern void nline (void);
extern void other (void);
extern void adjust (void);
extern int handle (int tok);

#define NLN() nline ()
#define OTR() other ()
#define ADJ() adjust ()
#define RET(TOK) return handle (TOK)

extern void error (ast_pos pos, const char *fmt, ...);

#endif