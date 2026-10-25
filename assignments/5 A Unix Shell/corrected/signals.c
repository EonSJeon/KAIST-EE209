/* CORRECTED: Assignment 5, Jeon Sangeon. Handlers only touch
   sig_atomic_t state and async-signal-safe alarm/write operations. */
#include "signals.h"
#include <errno.h>
#include <signal.h>
#include <unistd.h>
static volatile sig_atomic_t quitPending;
static volatile sig_atomic_t shouldQuit;

/* CORRECTED: Reset the five-second window without I/O or allocation. */
static void sigalrmHandler(int iSignal)
{
    int savedErrno = errno;
    (void)iSignal;
    quitPending = 0;
    errno = savedErrno;
}
/* CORRECTED: Arm the window or request exit, interrupting blocking I/O. */
static void sigquitHandler(int iSignal)
{
    static const char message[] =
        "\nType Ctrl-\\ again within 5 seconds to exit.\n";
    int savedErrno = errno;
    (void)iSignal;
    if (quitPending) { shouldQuit = 1; alarm(0); }
    else {
        quitPending = 1;
        (void)write(STDOUT_FILENO, message, sizeof(message) - 1);
        alarm(5);
    }
    errno = savedErrno;
}
/* CORRECTED: Unblock all three signals in the calling process. */
static int unblockSignals(void)
{
    sigset_t set;
    if (sigemptyset(&set) == -1 || sigaddset(&set, SIGINT) == -1 ||
        sigaddset(&set, SIGQUIT) == -1 || sigaddset(&set, SIGALRM) == -1)
        return -1;
    return sigprocmask(SIG_UNBLOCK, &set, NULL);
}
/* CORRECTED: Install using required signal(), then clear SA_RESTART
   so the second SIGQUIT interrupts fgets/waitpid for cleanup. */
int installSignals(void)
{
    int numbers[] = {SIGQUIT, SIGALRM};
    int i;
    struct sigaction action;
    if (signal(SIGINT, SIG_IGN) == SIG_ERR ||
        signal(SIGQUIT, sigquitHandler) == SIG_ERR ||
        signal(SIGALRM, sigalrmHandler) == SIG_ERR) return -1;
    for (i = 0; i < 2; i++) {
        if (sigaction(numbers[i], NULL, &action) == -1) return -1;
        action.sa_flags &= ~SA_RESTART;
        if (sigaddset(&action.sa_mask, SIGQUIT) == -1 ||
            sigaddset(&action.sa_mask, SIGALRM) == -1 ||
            sigaction(numbers[i], &action, NULL) == -1) return -1;
    }
    return unblockSignals();
}
/* CORRECTED: A child must not inherit the shell's ignored SIGINT. */
int resetChildSignals(void)
{
    alarm(0);
    if (signal(SIGINT, SIG_DFL) == SIG_ERR ||
        signal(SIGQUIT, SIG_DFL) == SIG_ERR ||
        signal(SIGALRM, SIG_DFL) == SIG_ERR) return -1;
    return unblockSignals();
}
/* CORRECTED: Expose the handler flag without mutable public data. */
int quitRequested(void) { return shouldQuit != 0; }
