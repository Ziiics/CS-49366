/*****************************************************************************
  Description : Installs signal handlers for ^C and ^\
  Purpose     : To show how to install signals and the effect of the handlers
  Usage       : signal_demo3
                While it is running, enter ^C and ^\ a few times.
  Build with  : gcc -I../include -o signal_demo3 signal_demo3.c -L../lib -lspl

*****************************************************************************/

#include  "common_hdrs.h"
#include  <signal.h>

static char* progname;

void catch_sigint(int signum)
{
    printf("%s caught CTRL-C!\n", progname);
}

int main(int argc, char* argv[])
{
    progname = argv[0];
    printf("PID=%d\n", getpid());
    if ( SIG_ERR == signal( SIGINT,  catch_sigint ) )
        fatal_error(errno, "signal()");
    while (TRUE) continue;
    return 0;
}
