/*--------------------------------------------------------------------*/
/* sumarray.c                                                         */
/* Author: Bob Dondero                                                */
/*--------------------------------------------------------------------*/

#include <stdio.h>

enum { ARRAYSIZE = 100 };

static int aiNumbers[ARRAYSIZE];
static int iIndex;
static int iCount;
static int iSum;

/* Read up to ARRAYSIZE integers from stdin, and write to stdout the
   sum of those integers.  Return 0. */
int main(void)
{
	printf("How many integers?  ");
	scanf("%d", &iCount);

	for (iIndex = 0; iIndex < iCount; iIndex++)
		scanf("%d", &aiNumbers[iIndex]);
	
	iSum = 0;
	for (iIndex = 0; iIndex < iCount; iIndex++)
		iSum += aiNumbers[iIndex];
	
	printf("The sum is %d.\n", iSum);
	
	return 0;
}
