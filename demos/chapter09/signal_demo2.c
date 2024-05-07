/*****************************************************************************
  Description : Sets disposition of SIGINT AND SIGQUIT to SIG_IGN
  Purpose     : To show how to ignore signals.
  Usage       : signal_demo2
                   While it is running, enter ^C and ^\ characters.
  Build wit   : gcc -I../include -o signal_demo2 signal_demo2.c -L../lib -lspl
*****************************************************************************/

#include "common_hdrs.h"
#include <signal.h>

int main(int argc, char* argv[])
{
    int i;
    if ( SIG_ERR == signal( SIGINT,  SIG_IGN ))   /* ignore Ctrl-C */
        fatal_error(errno, "signal()");
    if ( SIG_ERR == signal( SIGQUIT, SIG_IGN ))  /* ignore Ctrl-\ */
        fatal_error(errno, "signal()");
    for( i = 10; i > 0; i-- ) {
        printf("Try to kill me with ^C or ^\\. "
               "Seconds remaining: %2d\n", i);
        sleep(1);
    }
    return 0;
}

