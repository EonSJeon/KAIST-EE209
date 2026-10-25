/*--------------------------------------------------------------------*/
/* testdupin.c                                                        */
/* Slightly Updated By: Asim                                          */
/* Original Author: Bob Dondero                                       */
/* The dup system call.                                               */
/*--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

enum {BUFFER_LENGTH = 100};

int main(int argc, char *argv[])
{
   int iFd;
   int iRet;
   char pcBuffer[BUFFER_LENGTH];

   iFd = open("tempfile", O_RDONLY);
   if (iFd == -1) {perror(argv[0]); exit(EXIT_FAILURE); }

   iRet = close(0);
   if (iRet == -1) {perror(argv[0]); exit(EXIT_FAILURE); }

   iRet = dup(iFd);
   if (iRet == -1) {perror(argv[0]); exit(EXIT_FAILURE); }

   iRet = close(iFd);
   if (iRet == -1) {perror(argv[0]); exit(EXIT_FAILURE); }

   scanf("%s", pcBuffer);

   /* Print the data to verify that the scanf worked. */

   printf("%s\n", pcBuffer);
   return 0;
}

/*--------------------------------------------------------------------*/

/* Sample execution:

$ gcc209 testdupin.c -o testdupin

$ testdupin
somedata

*/
