#ifndef STR_INCLUDED
#define STR_INCLUDED
#include <stddef.h> /* CORRECTED: size_t belongs to this standard C header. */

/* Part 1 */
/* CORRECTED: Return the length of pcSrc, excluding its terminating null byte.
   All parameters of this module must be non-NULL (asserted).
   Functions perform no I/O and use no mutable global state. */
size_t StrGetLength(const char* pcSrc);
/* CORRECTED: Copy pcSrc, including its terminator, into pcDest; return pcDest.
   The caller supplies enough space and non-overlapping strings. */
char *StrCopy(char *pcDest, const char* pcSrc);
/* CORRECTED: Compare bytes as unsigned char; return negative, zero or positive. */
int StrCompare(const char* pcS1, const char* pcS2);
/* CORRECTED: Return the first match of pcNeedle, or NULL; empty needle matches
   the beginning of pcHaystack. The returned storage is not owned. */
char *StrSearch(const char* pcHaystack, const char *pcNeedle);
/* CORRECTED: Append pcSrc to pcDest and return pcDest. The caller provides enough
   space; the source and destination must not overlap. */
char *StrConcat(char *pcDest, const char* pcSrc);

#endif /* STR_INCLUDED */
