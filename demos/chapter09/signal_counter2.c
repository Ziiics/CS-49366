/*****************************************************************************
  Description : Counts and prints SIGINTs delivered a different way
  Purpose     : To show how to share data between handler and rest of program
  Usage       : signal_counter2
                Enter CTRL-Cs and observe that they're counted correctly.
  Build with  : gcc -I../include -o signal_counter2 signal_counter2.c \
                     -L../lib -lspl
*****************************************************************************/
#include  "common_hdrs.h"
#include  <signal.h>

static volatile sig_atomic_t count = 0;

void catch_sigint(int signum)
{
    count++;
}

int main (int argc, char* argv[])
{
    sigset_t blockedset;
    int i;
    int prev_count = 0;
    /* Initialize the signal mask. */
    sigemptyset (&blockedset);
    sigaddset (&blockedset, SIGINT);

    if ( SIG_ERR == signal(SIGINT, catch_sigint) )
        fatal_error(errno, "signal()");

    while (TRUE)    {
        /* Block SIGINT while we print the count. */
        if ( -1 == sigprocmask (SIG_BLOCK, &blockedset, NULL) )
            fatal_error(errno, "signal()");

        printf("\n%d more SIGINTs received\n", count-prev_count);
        prev_count = count;
        /* Unblock SIGINT, allowing handler to run. */
        if ( -1 == sigprocmask (SIG_UNBLOCK, &blockedset, NULL) )
            fatal_error(errno, "signal()");

        /* Delay to allow user to enter more CTRL-Cs.    */
        for ( i = 0; i < 100; i++) usleep(20000);
    }
}
