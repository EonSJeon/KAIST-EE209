#include <stdio.h>

/* Addition */
static int add(int i1, int i2)
{
    return i1 + i2;
}
/* Multiplication */
static int mul(int i1, int i2)
{
    return i1 * i2;
}
/* Test function pointer. */
int main(void)
{
    int a = 3, b = 5;
    int (*op)(int, int);

    op = add;
    printf("Add: %d\n", (*op)(a, b));
    op = mul;
    printf("Muliply: %d\n", (*op)(a, b));
    return 0;
}
