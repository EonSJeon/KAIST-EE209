/* CORRECTED: Assignment 5, Jeon Sangeon. Signal interface. */
#ifndef SIGNALS_INCLUDED
#define SIGNALS_INCLUDED
/* CORRECTED: Install/unblock handlers; return 0 or -1 with errno. */
int installSignals(void);
/* CORRECTED: Restore child defaults/unblock; return 0 or -1. */
int resetChildSignals(void);
/* CORRECTED: Return whether a second SIGQUIT requested clean exit. */
int quitRequested(void);
#endif
