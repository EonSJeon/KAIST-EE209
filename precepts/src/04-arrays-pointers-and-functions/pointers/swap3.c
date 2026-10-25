/* swap3.c */
#include <stdio.h>
#include <assert.h>

/* Swap two pointers to any type of arrays. */
static void swap(const void **p1, const void **p2)
{
    assert(p1 != NULL && p2 != NULL);
    const void *temp = *p1;
    *p1 = *p2;
    *p2 = temp;
    return;
}
/* Test swap function. */
int main(void)
{
    /* Swap strings. */
    const char *str1 = "KAIST";
    const char *str2 = "Welcome";
    printf("Before:  %s %s\n", str1, str2);
    swap((const void **)&str1, (const void **)&str2);
    printf("After:   %s %s\n", str1, str2);

    /* Swap array of integers. */
    int arr1[3] = {1, 2, 3};
    int arr2[3] = {4, 5, 6};
    printf("Before:  {%d, %d, %d} {%d, %d, %d}\n",
           arr1[0], arr1[1], arr1[2], arr2[0], arr2[1], arr2[2]);
    swap((const void **)&arr1, (const void **)&arr2);
    printf("After:   {%d, %d, %d} {%d, %d, %d}\n",
           arr1[0], arr1[1], arr1[2], arr2[0], arr2[1], arr2[2]);
    return 0;
}
