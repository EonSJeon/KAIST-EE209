/*--------------------------------------------------------------------*/
/* powerunsigned.c                                                    */
/* Author: Bob Dondero                                                */
/* Modified by Younghwan Go                                           */
/*--------------------------------------------------------------------*/

#include <stdio.h>

static unsigned int uiBase;
static unsigned int uiExp;
static unsigned int uiPower = 1;
static unsigned int uiIndex;

/* Read a non-negative base and exponent from stdin.  Write base
   raised to the exponent power to stdout.  Return 0. */
int main(void)
{
	printf("Enter the base:  ");
	scanf("%u", &uiBase);
	
	printf("Enter the exponent:  ");
	scanf("%u", &uiExp);
	
	for (uiIndex = 1; uiIndex <= uiExp; uiIndex++)
		uiPower *= uiBase;
	
	printf("%u to the %u power is %u.\n", uiBase, uiExp, uiPower);
	
	return 0;
}
