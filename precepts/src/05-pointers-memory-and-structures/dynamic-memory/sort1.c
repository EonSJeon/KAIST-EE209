/*--------------------------------------------------------------------*/
/* sort1.c                                                            */
/* Author: Bob Dondero                                                */
/* Modified by Myunggil Kang                                          */
/* A baseline example                                                 */
/*--------------------------------------------------------------------*/

#include <stdio.h>
#include <assert.h>

enum {ARRAY_LENGTH = 10};

/*--------------------------------------------------------------------*/

static void insertionSort(double *pdNumbers, int iCount)

/* Sort pdNumbers[0..iCount-1] in ascending order. */

{
   int i1;
   int i2;
   double dTemp;

   assert(pdNumbers != NULL);
   assert(iCount >= 0);

   for (i1 = 1; i1 < iCount; i1++)
      for (i2 = i1; i2 > 0; i2--)
         if (pdNumbers[i2] < pdNumbers[i2-1])
         {
            dTemp = pdNumbers[i2];
            pdNumbers[i2] = pdNumbers[i2-1];
            pdNumbers[i2-1] = dTemp;
         }
}

/*--------------------------------------------------------------------*/

int main(void)

/* Read numbers from stdin, and write them in ascending order to
   stdout.  Return 0. */

{
   double adNumbers[ARRAY_LENGTH];
   int iCount;
   double dNumber;
   int i;

   /* Read the numbers into an array. */

   iCount = 0;
   while (scanf("%lf", &dNumber) != EOF)
   {
      adNumbers[iCount] = dNumber;
      iCount++;
      if (iCount == ARRAY_LENGTH)
         break;
   }

   /* Sort the array. */

   insertionSort(adNumbers, iCount);

   /* Write the numbers from the array. */

   for (i = 0; i < iCount; i++)
      printf("%g\n", adNumbers[i]);

   return 0;
}
