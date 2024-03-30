#ifndef LEXER_H
#define LEXER_H

extern void lexer_nline (void);
extern void lexer_other (void);
extern void lexer_chpos (void);
extern int lexer_ret (int kind);

#define NLN() lexer_nline ()
#define OTR() lexer_other ()
#define ADJ() lexer_chpos ()
#define RET(kind) return lexer_ret (kind)

#endif