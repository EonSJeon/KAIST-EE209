/*--------------------------------------------------------------------*/
/* testsignalignore.c                                                 */
/* Slightly Updated By: Asim                                          */
/* Original Author: Bob Dondero                                       */
/* Ignoring signals.                                                  */
/*--------------------------------------------------------------------*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>

int main(int argc, char *argv[])
{
   void (*pfRet)(int);
   sigset_t sSet;
   int iRet;

   /* Make sure SIGINT signals are not blocked. */
   sigemptyset(&sSet);
   sigaddset(&sSet, SIGINT);
   iRet = sigprocmask(SIG_UNBLOCK, &sSet, NULL);
   assert(iRet == 0);

   /* Ignore SIGINT signals. */
   pfRet = signal(SIGINT, SIG_IGN);
   if (pfRet == SIG_ERR) {perror(argv[0]); exit(EXIT_FAILURE); }

   /* Enter an infinite loop. */
   printf("Entering an infinite loop\n");
   for (;;)
      ;

   /* Never should reach this point. */
}

/*--------------------------------------------------------------------*/

/* Sample execution:

$ gcc209 testsignalignore.c -o testsignalignore

$ testsignalignore
Entering an infinite loop
^c^c^c

*/

/* Note:  Can use kill command or Ctrl-\ to stop process. */
