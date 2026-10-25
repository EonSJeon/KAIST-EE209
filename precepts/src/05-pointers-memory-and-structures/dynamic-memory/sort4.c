/*--------------------------------------------------------------------*/
/* sort4.c                                                            */
/* Author: Bob Dondero                                                */
/* Modified by Myunggil Kang                                          */
/* A dynamically expanding array                                      */
/*--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

enum {INITIAL_ARRAY_LENGTH = 2};
enum {ARRAY_GROWTH_FACTOR = 2};

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
   stdout.  Return 0 iff successful. */

{
   double *pdNumbers;
   int iCount;
   int iArrayLength;
   double dNumber;
   int i;

   /* Dynamically allocate an array. */

   iArrayLength = INITIAL_ARRAY_LENGTH;
   pdNumbers = (double*)calloc((size_t)iArrayLength, sizeof(double));
   if (pdNumbers == NULL)
   {
      fprintf(stderr, "Cannot allocate memory\n");
      exit(EXIT_FAILURE);
   }

   /* Read the numbers into the array. */

   iCount = 0;
   while (scanf("%lf", &dNumber) != EOF)
   {
      if (iCount == iArrayLength)
      {
         iArrayLength *= ARRAY_GROWTH_FACTOR;
         pdNumbers =
            (double*)realloc(pdNumbers, iArrayLength * sizeof(double));
         if (pdNumbers == NULL)
         {
            fprintf(stderr, "Cannot allocate memory\n");
            exit(EXIT_FAILURE);
         }
      }
      pdNumbers[iCount] = dNumber;
      iCount++;
   }

   /* Sort the array. */

   insertionSort(pdNumbers, iCount);

   /* Write the numbers from the array. */

   for (i = 0; i < iCount; i++)
      printf("%g\n", pdNumbers[i]);

   /* Free the array. */

   free(pdNumbers);

   return 0;
}
