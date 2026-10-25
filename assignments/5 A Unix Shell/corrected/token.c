/* CORRECTED: Assignment 5, Jeon Sangeon. Original token helpers split
   out of ish.c; release ownership on every allocation failure. */
#include "token.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* CORRECTED: Copy pcValue into a new token; return NULL on failure. */
struct Token *makeToken(enum TokenType eType, const char *pcValue)
{
    struct Token *psToken;
    assert(pcValue != NULL);
    psToken = malloc(sizeof(*psToken));
    if (psToken == NULL) return NULL;
    psToken->pcValue = malloc(strlen(pcValue) + 1);
    if (psToken->pcValue == NULL) { free(psToken); return NULL; }
    psToken->eType = eType;
    strcpy(psToken->pcValue, pcValue);
    return psToken;
}
/* CORRECTED: Free one owned token; ignore the optional map argument. */
void freeToken(void *pvItem, void *pvExtra)
{
    struct Token *psToken = pvItem;
    assert(psToken != NULL);
    (void)pvExtra;
    free(psToken->pcValue);
    free(psToken);
}
/* CORRECTED: Release both ownership levels; perform no I/O. */
void freeTokens(DynArray_T oTokens)
{
    if (oTokens == NULL) return;
    DynArray_map(oTokens, freeToken, NULL);
    DynArray_free(oTokens);
}
