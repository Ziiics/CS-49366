/*****************************************************************************
  Description    : Installs signal handlers for ^C and ^\
  Purpose        : To show how to install signal handlers and see their effect
  Usage          : signal_demo1
                   While it is running, enter ^C and ^\ a few times.
  Build with     : gcc -Wall -g -o signal_demo1 signal_demo1.c
****************************************************************************/

#include "common_hdrs.h"
#include  <signal.h>

/****************************************************************************/
/*                                Handlers                                  */
/*****************************************************************************/
void catch_sigint(int signum)
{
    printf("I'm not killed by CTRL-C!\n");
}

void catch_sigquit(int signum)
{
    printf("I'm not killed by CTRL-\\!\n");
}

int main()
{
    int	i;
    if ( SIG_ERR == signal( SIGINT,  catch_sigint) )
        fatal_error(errno, "signal()");
    if ( SIG_ERR == signal( SIGQUIT, catch_sigquit) )
        fatal_error(errno, "signal()");
    for(i = 20; i > 0; i-- ) {
        printf("Try to terminate me with ^C or ^\\.\n");
        sleep(1);
    }
    return 0;
}
