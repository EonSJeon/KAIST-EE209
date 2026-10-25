/* swap2.c */
#include <stdio.h>
#include <assert.h>

/* Swap two pointers to strings. */
static void swap(const char **p1, const char **p2)
{
    assert(p1 != NULL && p2 != NULL);
    const char *temp = *p1;
    *p1 = *p2;
    *p2 = temp;
    return;
}
/* Test swap function. */
int main(void)
{
    const char *str1 = "KAIST";
    const char *str2 = "Welcome";
    printf("Before:  %s %s\n", str1, str2);
    swap(&str1, &str2);
    printf("After:   %s %s\n", str1, str2);
    return 0;
}
