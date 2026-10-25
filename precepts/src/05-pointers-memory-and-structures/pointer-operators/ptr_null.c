/*--------------------------------------------------------------------*/
/* ptr_null.c                                                         */
/* Author: Bob Dondero                                                */
/* Modified by Younghwan Go                                           */
/* A Nonsensical Program to Illustrate Pointers.                      */
/*--------------------------------------------------------------------*/

#include <stdio.h>

int main(void)
{
   int i1;      
   int *pi3;    
                
   i1 = 5;
   pi3 = &i1;   /* "&" is the "address of" operator. */

   pi3 = NULL;  /* Indicates that pi3 points to no valid memory
                   location. */

   /* NULL is a #defined constant in several standard header files. */
   /* #define NULL (void*)0 */

   /* *pi3 = 8;    Runtime error: Seg fault. */
   /* Note that NULL differs from "unpredictable value." */

   pi3 = &i1;   /* Restore value of pi3 */
   printf("i1: %d, *pi3: %d\n", i1, *pi3);

   return 0;
}
