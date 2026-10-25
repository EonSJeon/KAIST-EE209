/*--------------------------------------------------------------------*/
/* testdupout.c                                                       */
/* Slightly Updated By: Asim                                          */
/* Original Author: Bob Dondero                                       */
/* The dup system call.                                               */
/*--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
   int iFd;
   int iRet;

   iFd = creat("tempfile", 0600);
   if (iFd == -1) {perror(argv[0]); exit(EXIT_FAILURE); }

   iRet = close(1);
   if (iRet == -1) {perror(argv[0]); exit(EXIT_FAILURE); }

   iRet = dup(iFd);
   if (iRet == -1) {perror(argv[0]); exit(EXIT_FAILURE); }

   iRet = close(iFd);
   if (iRet == -1) {perror(argv[0]); exit(EXIT_FAILURE); }

   printf("somedata\n");

   return 0;
}

/*--------------------------------------------------------------------*/

/* Sample execution:

$ gcc209 testdupout.c -o testdupout

$ testdupout

$ cat tempfile
somedata

*/
