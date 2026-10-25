/* CORRECTED: Assignment 5, Jeon Sangeon. Stateless DFA lexer. */
#ifndef LEXER_INCLUDED
#define LEXER_INCLUDED
#include "token.h"
/* CORRECTED: Append pcLine tokens to oTokens; return an error string
   or NULL. Caller frees tokens on either outcome. No I/O. */
const char *lexLine(const char *pcLine, DynArray_T oTokens);
#endif
