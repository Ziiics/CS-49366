/*****************************************************************************
  Description/   : Shows how to share global flag for printing messages
  Purpose        :
  Usage          : signal_demo4
  Build with  : gcc -o signal_demo4 -I../include -L../lib signal_demo4.c -lspl
*****************************************************************************/
#include  "common_hdrs.h"
#include  <signal.h>

static volatile sig_atomic_t flag = 0;

void catch_sigint(int signum)
{
    flag = 1;
}

int main (void)
{
    sigset_t blockedset;
    struct sigaction  sig_act;

    /* Initialize the signal mask. */
    sigemptyset (&blockedset);
    sigaddset (&blockedset, SIGINT);
    sig_act.sa_handler = catch_sigint;
    sig_act.sa_flags = 0;

    if ( -1 == sigaction(SIGINT, &sig_act, NULL) )
        fatal_error(errno, "sigaction()");
    while (TRUE)    {
        /* Check if a signal has arrived; if so, reset the flag. */
        if ( -1 == sigprocmask (SIG_BLOCK, &blockedset, NULL) )
            fatal_error(errno, "sigprocmask()");
        if ( flag ) {
             printf("SIGINT received\n");
             flag = 0;
        }
        else
            printf("SIGINT not received\n");

        if ( -1 == sigprocmask (SIG_UNBLOCK, &blockedset, NULL) )
            fatal_error(errno, "sigprocmask()");
        sleep(4);
    }
}

