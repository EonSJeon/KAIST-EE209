/* CORRECTED: Assignment 5, Jeon Sangeon. Foreground execution. */
#ifndef EXECUTE_INCLUDED
#define EXECUTE_INCLUDED
#include "parser.h"
/* CORRECTED: Run plan; set *exitRequested for exit builtin. Children
   use stdio; errors go to stderr prefixed by pgmName. */
void executeLine(const struct Pipeline *plan, const char *pgmName,
                 int *exitRequested);
#endif
