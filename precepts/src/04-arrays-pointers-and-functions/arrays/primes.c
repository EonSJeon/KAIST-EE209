/* primes.c */
#include <stdio.h>

/* Calculate prime numbers and write in an array. */
int main(void)
{
    int primes[16];
    int length = sizeof(primes)/sizeof(primes[0]);  // length of array
    int i, idx = 0;
    int candidate = 2;  // candidate for the next prime number
    do {
        for (i = 0; i < idx; i++) {
            if (candidate % primes[i] == 0) {
                candidate++; i = -1;
            }
        }
        primes[idx++] = candidate++;    // candidate selected
    } while (idx < length);

    /* Print results */
    printf("%d Prime numbers: ", length);
    for (idx = 0; idx < length; idx++) {
        printf("%d ", primes[idx]);
    }
    printf("\n");
    return 0;
}
