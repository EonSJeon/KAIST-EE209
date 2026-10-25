/*--------------------------------------------------------------------*/
/* powerfunctionflat.c                                                */
/* Author: Bob Dondero                                                */
/*--------------------------------------------------------------------*/

#include <stdio.h>

/*--------------------------------------------------------------------*/
/* Return iBase raised to the iExp power, where iBase and iExp
   are non-negative. */
static int power(int iBase, int iExp)
{
	int iPower = 1;
	int iIndex;
	
	iIndex = 1;

 loop1:
	if (iIndex > iExp) goto loopend1;
	iPower *= iBase;
	iIndex++;
	goto loop1;

 loopend1:
	
	return iPower;
}
/*--------------------------------------------------------------------*/
/* Read a non-negative base and exponent from stdin.  Write base
   raised to the exponent power to stdout.  Return 0. */
int main(void)
{
	int iBase;
	int iExp;
	int iPower;
	
	printf("Enter the base:  ");
	scanf("%d", &iBase);
	
	printf("Enter the exponent:  ");
	scanf("%d", &iExp);
	
	iPower = power(iBase, iExp);
	
	printf("%d to the %d power is %d.\n", iBase, iExp, iPower);
	
	return 0;
}
