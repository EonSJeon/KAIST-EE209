/* CORRECTED: Assignment 5, Jeon Sangeon. Validated execution plan. */
#ifndef PARSER_INCLUDED
#define PARSER_INCLUDED
#include "token.h"
enum CommandType {USER, SETENV, UNSETENV, CD, EXIT};
struct Command {
    char **pgmArgs;          /* CORRECTED: owned argv, borrowed strings. */
    int argc;               /* CORRECTED: excludes NULL sentinel. */
    enum CommandType type;  /* CORRECTED: USER or builtin category. */
    const char *inputFile;  /* CORRECTED: borrowed name or NULL. */
    const char *outputFile; /* CORRECTED: borrowed name or NULL. */
};
struct Pipeline {
    struct Command *commands; /* CORRECTED: owned command array. */
    int count;                /* CORRECTED: number of stages. */
};
/* CORRECTED: Return the builtin category of pcName. No I/O. */
enum CommandType commandType(const char *pcName);
/* CORRECTED: Parse oTokens into a zero-initialized plan. Return error
   or NULL. Free plan on both success and failure. No I/O. */
const char *synLine(DynArray_T oTokens, struct Pipeline *plan);
/* CORRECTED: Release plan arrays; token strings remain lexer-owned. */
void freePipeline(struct Pipeline *plan);
#endif
