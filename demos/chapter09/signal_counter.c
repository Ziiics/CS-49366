/******************************************************************************
  Description    : Records number of SIGINT signals received by process
  Purpose        : To show that at most one signal is pending of a given type
  Usage          : signal_counter
                   Run the program, enter CTRL-Cs and then CTRL-\
  Build with     : gcc -Wall -DDELAY=20 -g -I../include -o signal_counter  \
                    signal_counter.c  -L../lib -lspl
                   (Optionally define DELAY)
  Note           : This is not a safe way to count signals because the
                   increment to count in the handler may be a race condition
                   in certain implementations of signal().
*****************************************************************************/
#include  "common_hdrs.h"
#include  <signal.h>

static volatile sig_atomic_t count = 0;
static volatile sig_atomic_t stop = 0;

#ifdef DELAY
    #define NDELAY  DELAY
#else
    #define NDELAY  20
#endif

void delay(int n)
{
   int i, iterations = 100*n;
   for ( i = 0; i < iterations; i++ ) continue;
}

void catch_sigint(int signum)
{
    count++;
    delay(NDELAY);
}

void catch_sigquit( int signum )
{
    stop = 1;
}

int main()
{
    if ( SIG_ERR == signal( SIGINT, catch_sigint ) )
        fatal_error(errno, "signal()");
    if ( SIG_ERR == signal( SIGQUIT, catch_sigquit ) )
        fatal_error(errno, "signal()");
    printf("Enter a SIGQUIT (Ctrl-\\) to end this program!\n");
    while ( !stop ) continue;
    printf("\n%d SIGINT signals received \n", count);
    return 0;
}
