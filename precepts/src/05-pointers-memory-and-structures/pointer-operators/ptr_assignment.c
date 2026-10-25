/*--------------------------------------------------------------------*/
/* ptr_assignment.c                                                             */
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
   int* pi4;    
                
   i1 = 5;
   printf("i1 = 5\t\t->\ti1: %d\n", i1);

   /* pi4 = 6;     Compiletime warning: type mismatch. */
   /* pi3 = i1;    Compiletime warning: type mismatch. */
   /* i1 = pi3;    Compiletime warning: type mismatch. */

   pi3 = &i1;   /* "&" is the "address of" operator. */
   printf("pi3 = &i1\t->\ti1: %d, *pi3: %d\n", i1, *pi3);

   /* pi3 = 6;     Still compiletime warning. */
   *pi3 = 6;       /* "*" is the "dereference" operator. */
                   /* Changes value of *pi3 and i1. */
                   /* *pi3 and i1 are aliases. */
   printf("*pi3 = 6\t->\ti1: %d, *pi3: %d\n", i1, *pi3);

   /* *pi4 = 7;    Runtime error. Seg fault, or memory corruption. */

   pi4 = &i2;      /* Hereafter, *pi4 and i2 are aliases. */
   printf("pi4 = &i2\t->\ti2: %d, *pi4: %d\n", i2, *pi4);


   i2 = *pi3;      /* Assigns one int to another. */
   printf("i2 = *pi3\t->\ti1: %d, *pi3: %d, i2: %d, *pi4: %d\n", i1, *pi3, i2, *pi4);

   i2 = 4;
   printf("i2 = 4\t\t->\ti1: %d, *pi3: %d, i2: %d, *pi4: %d\n", i1, *pi3, i2, *pi4);

   *pi4 = *pi3;    /* Same as previous. Assigns one int to another. */
   printf("*pi4 = *pi3\t->\ti1: %d, *pi3: %d, i2: %d, *pi4: %d\n", i1, *pi3, i2, *pi4);

   i2 = 4;
   printf("i2 = 4\t\t->\ti1: %d, *pi3: %d, i2: %d, *pi4: %d\n", i1, *pi3, i2, *pi4);

   pi4 = pi3;      /* Assigns one address to another. */
                   /* *pi3 and *pi4 are now aliases. */
   printf("pi4 = pi3\t->\ti1: %d, *pi3: %d, i2: %d, *pi4: %d\n", i1, *pi3, i2, *pi4);

   pi4 = &i2;      /* Restore pi4 to previous value */
   printf("pi4 = &i2\t->\ti1: %d, *pi3: %d, i2: %d, *pi4: %d\n", i1, *pi3, i2, *pi4);

   /* Note that & and * are inverse operators:
	  &*pi3 is the same as pi3
	  *&i1  is the same as i1  */

   return 0;
}
