/* CORRECTED: Assignment 5, Jeon Sangeon. Token ownership interface. */
#ifndef TOKEN_INCLUDED
#define TOKEN_INCLUDED
#include "dynarray.h"
enum {MAX_LINE_SIZE = 1024};
enum TokenType {TOKEN_ORDINARY, TOKEN_SPECIAL};
struct Token {
    enum TokenType eType; /* CORRECTED: quoted symbols stay ordinary. */
    char *pcValue;        /* CORRECTED: owned null-terminated string. */
};
/* CORRECTED: Copy pcValue to a new token; return NULL on failure. */
struct Token *makeToken(enum TokenType eType, const char *pcValue);
/* CORRECTED: Free pvItem and its string; pvExtra is unused. No I/O. */
void freeToken(void *pvItem, void *pvExtra);
/* CORRECTED: Free all tokens and their container; accept NULL. */
void freeTokens(DynArray_T oTokens);
#endif
