/*****************************************************************************
  Description : Blocks all possible signals (except SIGKILL and SIGSTOP)
  Purpose     : To show how blocking works
  Usage       : sigprocmask_demo2
                Open a second window, and use kill to send different
                signals to this process.
  Build with  : gcc -I../include -o sigprocmask_demo2 sigprocmask_demo2.c \
                     -L../lib -lspl
*****************************************************************************/

#include  "common_hdrs.h"
#include  <signal.h>

int main(int argc, char* argv[])
{
    sigset_t signals, prevsignals;

    printf("PID=%d\n",getpid());
    sigfillset(&signals);
    if ( -1 == sigprocmask(SIG_BLOCK, &signals, &prevsignals) )
        fatal_error(errno, "sigprocmask()");

    while ( TRUE ) {
        printf("Try sending signals to me. "
               "Use SIGKILL to terminate me, SIGSTOP to stop me.\n");
        sleep(5);
    }
    if ( -1 == sigprocmask(SIG_SETMASK, &prevsignals, NULL) )
        fatal_error(errno, "sigprocmask()");
    return 0;
}

