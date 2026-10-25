/*--------------------------------------------------------------------*/
/* powerflat.c                                                        */
/* Author: Bob Dondero                                                */
/* Modified by Younghwan Go                                           */
/*--------------------------------------------------------------------*/

#include <stdio.h>

static int iBase;
static int iExp;
static int iPower = 1;
static int iIndex;

/* Read a non-negative base and exponent from stdin.  Write base
   raised to the exponent power to stdout.  Return 0. */
int main(void)
{
	printf("Enter the base:  ");
	scanf("%d", &iBase);
	
	printf("Enter the exponent:  ");
	scanf("%d", &iExp);
	
	iIndex = 1;

 loop1:
	if (iIndex > iExp) 
		goto loopend1;
	iPower *= iBase;
	iIndex++;
	goto loop1;

 loopend1:
	printf("%d to the %d power is %d.\n", iBase, iExp, iPower);
	
	return 0;
}
