/* swap1.c */
#include <stdio.h>

/* Test swapping two addresses. */
int main(void)
{
    const char *str1 = "KAIST";
    const char *str2 = "Welcome";
    const char *temp;
    printf("Before:  %s %s\n", str1, str2); /* KAIST Welcome */

    /* swap */
    temp = str1;
    str1 = str2;
    str2 = temp;
    printf("After:   %s %s\n", str1, str2); /* Welcome KAIST */
    return 0;
}
