/*****************************************************************************
  Description : Tries to install universal signal handler for all signals
  Purpose     : To show how to install one handler for all signals
  Usage       : signal_demo5
                   Observe the error messages.
  Build with  : gcc -I../include -o signal_demo5 signal_demo5.c -L../lib -lspl
*****************************************************************************/
#define _GNU_SOURCE
#include  "common_hdrs.h"
#include  <signal.h>

void all_signal_handler(int signum)
{
    printf("Caught signal SIG%s\n", sigabbrev_np(signum)); /* UNSAFE */
}

int main()
{
    int	i;
    printf("PID=%d\n", getpid());
    for ( i = 1; i < NSIG; i++ )  // NSIG represents the total number of signals defined in the system, both standard and additional implementation-defined signals
        if ( SIG_ERR == signal( i,  all_signal_handler ))
            printf("Could not install handler for signal SIG%s, (%d)\n",
                   sigabbrev_np(i), i);
    printf("Use SIGKILL to terminate this program.\n");
    while ( TRUE ) continue;
    return 0;
}
