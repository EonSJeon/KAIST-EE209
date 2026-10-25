/* Supplemental implementation for the incomplete precept main.
   Assumption: words contain letters, numbers contain digits, whitespace
   separates tokens, and mixed/other characters make a line invalid.
   This is an exercise completion, not a recovered original solution. */
#include "precept_lexer.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
enum TokenType {TOKEN_NUMBER, TOKEN_WORD};
struct Token {enum TokenType eType; char *pcValue;};

/* Append tokens owned by the caller, even on a partial failure. */
int lexLine(const char *line, DynArray_T tokens)
{
    const char *cursor = line;
    assert(line != NULL && tokens != NULL);
    while (*cursor) {
        const char *start;
        size_t length;
        struct Token *token;
        enum TokenType type;
        while (isspace((unsigned char)*cursor)) cursor++;
        if (!*cursor) break;
        start = cursor;
        type = isdigit((unsigned char)*cursor) ? TOKEN_NUMBER : TOKEN_WORD;
        while (*cursor && !isspace((unsigned char)*cursor)) {
            if (type == TOKEN_NUMBER ? !isdigit((unsigned char)*cursor)
                                     : !isalpha((unsigned char)*cursor))
                return 0;
            cursor++;
        }
        length = (size_t)(cursor - start);
        token = malloc(sizeof(*token));
        if (token == NULL) return 0;
        token->pcValue = malloc(length + 1);
        if (token->pcValue == NULL) { free(token); return 0; }
        memcpy(token->pcValue, start, length);
        token->pcValue[length] = '\0';
        token->eType = type;
        if (!DynArray_add(tokens, token)) { freeToken(token, NULL); return 0; }
    }
    return 1;
}
void printNumberToken(void *item, void *extra)
{
    struct Token *token = item;
    assert(token != NULL); (void)extra;
    if (token->eType == TOKEN_NUMBER) printf("%s ", token->pcValue);
}
void printWordToken(void *item, void *extra)
{
    struct Token *token = item;
    assert(token != NULL); (void)extra;
    if (token->eType == TOKEN_WORD) printf("%s ", token->pcValue);
}
void freeToken(void *item, void *extra)
{
    struct Token *token = item;
    assert(token != NULL); (void)extra;
    free(token->pcValue); free(token);
}
