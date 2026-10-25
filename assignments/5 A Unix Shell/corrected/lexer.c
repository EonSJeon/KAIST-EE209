/* CORRECTED: Assignment 5, Jeon Sangeon. Preserve the original DFA
   approach, with one word state after closing a quote. */
#include "lexer.h"
#include <assert.h>
#include <ctype.h>
#include <stddef.h>

/* CORRECTED: Append an owned token; return 0 on allocation failure. */
static int makeAddToken(enum TokenType type, const char *value,
                        DynArray_T oTokens)
{
    struct Token *psToken;
    assert(value != NULL);
    assert(oTokens != NULL);
    psToken = makeToken(type, value);
    if (psToken == NULL) return 0;
    if (!DynArray_add(oTokens, psToken)) {
        freeToken(psToken, NULL);
        return 0;
    }
    return 1;
}
/* CORRECTED: Tokenize without I/O; NULL means success. Empty quoted
   strings form words, adjacent quoted fragments concatenate. */
const char *lexLine(const char *pcLine, DynArray_T oTokens)
{
    enum LexState {OUT_WORD, IN_WORD, IN_QUOTE};
    enum LexState eState = OUT_WORD;
    size_t iLineIndex = 0, iValueIndex = 0;
    char acValue[MAX_LINE_SIZE];
    assert(pcLine != NULL);
    assert(oTokens != NULL);
    for (;;) {
        unsigned char c = (unsigned char)pcLine[iLineIndex++];
        int special = c == '|' || c == '<' || c == '>';
        if (eState == IN_QUOTE) {
            if (c == '\n' || c == '\0')
                return "Could not find quote pair";
            if (c == '"') { eState = IN_WORD; continue; }
        }
        else {
            if (c == '"') { eState = IN_QUOTE; continue; }
            if (c == '\0' || isspace(c) || special) {
                if (eState == IN_WORD) {
                    acValue[iValueIndex] = '\0';
                    if (!makeAddToken(TOKEN_ORDINARY, acValue, oTokens))
                        return "Cannot allocate memory";
                    iValueIndex = 0;
                    eState = OUT_WORD;
                }
                if (c == '\0' || c == '\n') return NULL;
                if (special) {
                    char symbol[2] = {(char)c, '\0'};
                    if (!makeAddToken(TOKEN_SPECIAL, symbol, oTokens))
                        return "Cannot allocate memory";
                }
                continue;
            }
            eState = IN_WORD;
        }
        if (iValueIndex >= sizeof(acValue) - 1)
            return "Input line is too long";
        acValue[iValueIndex++] = (char)c;
    }
}
