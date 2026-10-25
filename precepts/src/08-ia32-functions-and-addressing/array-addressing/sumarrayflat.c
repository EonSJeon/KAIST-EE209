/*--------------------------------------------------------------------*/
/* sumarrayflat.c                                                     */
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
	
	iIndex = 0;

loop1:
	if (iIndex >= iCount) 
		goto loopend1;
	scanf("%d", &aiNumbers[iIndex]);
	iIndex++;
	goto loop1;

loopend1:
	iSum = 0;
	iIndex = 0;

loop2:
	if (iIndex >= iCount) goto loopend2;
	iSum += aiNumbers[iIndex];
	iIndex++;
	goto loop2;

loopend2:
	printf("The sum is %d.\n", iSum);

   return 0;
}
