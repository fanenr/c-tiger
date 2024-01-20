#ifndef LEXER_H
#define LEXER_H

extern unsigned chpos;
extern unsigned lnpos;

extern void nline (void);
extern void other (void);
extern void adjust (void);
extern void error (const char *fmt, ...);
extern int handle (int tok);

#define NLN() nline ()
#define OTR() other ()
#define ADJ() adjust ()
#define RET(TOK) return handle (TOK)

#endif