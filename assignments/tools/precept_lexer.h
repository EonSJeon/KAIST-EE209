/* The archived precept dfa.c contains main only. These declarations
   supply its missing exercise helpers without modifying precepts. */
#ifndef PRECEPT_LEXER_INCLUDED
#define PRECEPT_LEXER_INCLUDED
#include "dynarray.h"
int lexLine(const char *line, DynArray_T tokens);
void printNumberToken(void *item, void *extra);
void printWordToken(void *item, void *extra);
void freeToken(void *item, void *extra);
#endif
