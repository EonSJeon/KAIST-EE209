/*--------------------------------------------------------------------*/
/* ptr_relational.c                                                   */
/* Author: Bob Dondero                                                */
/* Modified by Younghwan Go                                           */
/* A Nonsensical Program to Illustrate Pointers.                      */
/*--------------------------------------------------------------------*/

#include <stdio.h>

int main(void)
{
	int i1;      
	int i2;      
	int *pi3;    
	int *pi4;    
	
	i1 = 5;
	pi3 = &i1; 
	*pi3 = 6;  
	pi4 = &i2; 
	i2 = *pi3; 
	pi4 = &i2;  
	
	printf("i1: %d, *pi3: %d, i2: %d, *pi4: %d\n", i1, *pi3, i2, *pi4);
	
	if (*pi3 == *pi4)  /* Compares ints. Evaluates to TRUE (1). */
		printf("Integers are equal\n");
	
	if (pi3 == pi4)    /* Compares addresses. Evaluates to FALSE (0). */
	   printf("Pointers are equal\n");
	
	if (pi3 != pi4)    /* Compares addresses. Evaluates to TRUE (1). */
		printf("Pointers are NOT equal. pi3: %p, pi4: %p\n", (void *)pi3, (void *)pi4);
	
	/* Note:
	   if (pi3 == pi4) is TRUE,
	   then (*pi3 == *pi4) is TRUE.
	   if (*pi3 == *pi4) is TRUE,
	   then (pi3 == pi4) may or may not be TRUE.  */
	
	if (pi3 == NULL)   /* Compares addresses. Evaluates to FALSE (0). */
		printf("Pointer is NULL\n");
	
	return 0;
}
