/*--------------------------------------------------------------------*/
/* testsignal.c                                                       */
/* Slightly Updated By: Asim                                          */
/* Original Author: Bob Dondero                                       */
/* The signal system call.                                            */
/*--------------------------------------------------------------------*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>

/*--------------------------------------------------------------------*/

static void mySigintHandler(int iSignal)
{
   printf("In mySigintHandler with argument %d\n", iSignal);
}

/*--------------------------------------------------------------------*/

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

   /* Install mySigintHandler as the handler for SIGINT signals. */
   pfRet = signal(SIGINT, mySigintHandler);
   if (pfRet == SIG_ERR) {perror(argv[0]); exit(EXIT_FAILURE); }

   printf("Entering an infinite loop\n");
   for (;;)
      ;

   /* Never should reach this point. */
}

/*--------------------------------------------------------------------*/

/* Sample execution:

$ gcc209 testsignal.c -o testsignal

$ testsignal
Entering an infinite loop
^CIn mySigintHandler with argument 2
^CIn mySigintHandler with argument 2
^CIn mySigintHandler with argument 2

*/

/* Note:  Can use kill command or Ctrl-\ to stop process. */
