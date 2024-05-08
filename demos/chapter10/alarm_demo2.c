/*****************************************************************************
  Description : Put process to sleep for specified period.
  Purpose     : Show how alarm() behaves
  Usage       : alarm_demo2 [seconds]
                   If seconds is not supplied, the default is 5 seconds

  Build with  : gcc -Wall -g -I../include -L../lib -o alarm_demo2  \
                  alarm_demo2.c -lspl
*****************************************************************************/

/*

struct timespec {
  time_t tv_sec; // seconds
  /* ... */ tv_nsec; // nanoseconds
} // #include <time.h> dsecribes times in seconds and nanoseconds

unsigned int alarm(unsigned int seconds);
  // arrages SIGALARM signla to be delivered to the calling process in 'seconds' seconds
  // came as settimer(2)
  #include <unistd.h>

int nanosleep(const struct timespec *req, struct timespec *rem); 
  // suspend the execution of the calling thread for an interval specified with nanosec
  const struct timespec *req - a pointer to 'struct timespec' that specifies interval time of the thread should be suspended
  struct timespec *rem - pointer to 'struct timespec' where any remaining time will be store when the call is interrupted by signal handler
*/

#include "common_hdrs.h"
#include <signal.h>


#define DEFAULT_DELAY   (5)
char  MESSAGE[]= "Received a wake-up signal!\n";

/* The signal handler for SIGALRM */
void catchalarm(int signum)
{
    write(STDOUT_FILENO, MESSAGE, sizeof(MESSAGE));
    exit(EXIT_SUCCESS);      /* Exit the program */
}

int main(int argc, char* argv[])
{
    int delay = DEFAULT_DELAY;
    int retval = 0, k = 0;
    struct timespec sleeptime = {1,0};   /* 1 second */
    struct sigaction act;

    if ( argc >= 2) {
        retval = get_int(argv[1], NON_NEG_ONLY|PURE, &delay, NULL );  // convert comment line into integer
        if ( retval < 0 || delay < 1 )
            fatal_error(retval, "get_int expects a positive integer");
    }
    act.sa_handler = catchalarm;
    sigemptyset(&(act.sa_mask));
    sigaction(SIGALRM, &act, NULL);
    printf("About to sleep for %d seconds\n", delay);
    alarm(delay);	           /* turn on alarm */
    while ( TRUE ) {
        nanosleep(&sleeptime, NULL);
        printf("%d second(s) elapsed\n",++k);
    }
    return 0;
}
