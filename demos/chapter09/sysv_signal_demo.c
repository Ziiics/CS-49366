/******************************************************************************
  Description    : Sets dispositions for ^C and ^\ using System V semantics
  Purpose        : To show how System V signal handling works
  Usage          : sysv_signal_demo
                   While it is running, enter ^C and ^\ a few times.
  Build with     : gcc -o sysv_signal_demo -I../include -L../lib \
                   sysv_signal_demo.c -lspl
*****************************************************************************/
#define _GNU_SOURCE
#include  "common_hdrs.h"
#include  <signal.h>

/****************************************************************************/
/*                                Handlers                                  */
/*****************************************************************************/
void catch_sigint(int signum)
{
    /* REMINDER: printf() is not safe in the signal handler. */
    printf("I'm not killed by CTRL-C!\n");
}

void catch_sigquit(int signum)
{
    /* REMINDER: printf() is not safe in the signal handler. */
    printf("I'm not killed by CTRL-\\!\n");
}

int main()
{
    int	i;
    if ( SIG_ERR == sysv_signal( SIGINT,  catch_sigint ) )
        fatal_error(errno, "sysv_signal()");
    if ( SIG_ERR == sysv_signal( SIGQUIT, catch_sigquit ) )
        fatal_error(errno, "sysv_signal()");
    for (i = 20; i > 0; i-- ) {
        printf("Try to terminate me with ^C or ^\\.\n");
        sleep(1);
    }
    return 0;
}
