/* CORRECTED: Assignment 2 regression client, added for this review.
   Compare the module against libc, especially unsigned byte order. */
#include "str.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* CORRECTED: Normalize the only guaranteed property of strcmp. */
static int signOf(int value) { return (value > 0) - (value < 0); }

/* CORRECTED: Check return pointers, terminators, empty strings,
   repeated prefixes and all byte values against standard functions. */
int main(void)
{
    char destination[128], expected[128];
    char first[2] = {0, 0}, second[2] = {0, 0};
    const char *words[] = {"", "a", "aaa", "aab", "aaaaab", "hello world"};
    size_t i, j;
    int a, b;
    for (a = 0; a <= 255; a++)
        for (b = 0; b <= 255; b++) {
            first[0] = (char)a; second[0] = (char)b;
            assert(signOf(StrCompare(first, second)) ==
                   signOf(strcmp(first, second)));
        }
    for (i = 0; i < sizeof(words)/sizeof(*words); i++) {
        assert(StrGetLength(words[i]) == strlen(words[i]));
        assert(StrCopy(destination, words[i]) == destination);
        assert(strcmp(destination, words[i]) == 0);
        for (j = 0; j < sizeof(words)/sizeof(*words); j++) {
            const char *actual = StrSearch(words[i], words[j]);
            const char *standard = strstr(words[i], words[j]);
            assert(actual == standard);
            strcpy(destination, words[i]); strcpy(expected, words[i]);
            assert(StrConcat(destination, words[j]) == destination);
            strcat(expected, words[j]);
            assert(strcmp(destination, expected) == 0);
        }
    }
    puts("Str: all 65536 byte comparisons and string cases passed");
    return 0;
}
