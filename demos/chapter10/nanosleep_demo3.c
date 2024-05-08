/*****************************************************************************
  Description : Display a blinking shape during process sleep
  Purpose     : Shows one way to create flashing cursor
  Usage       : nanosleep_demo3 [seconds]
                   If seconds is not supplied, the default is 5 seconds
  Build with  : gcc -Wall -g -I../include -L../lib -o nanosleep_demo3  \
                  nanosleep_demo3.c -lspl
*****************************************************************************/

/*

int fflush(FILE *_Nullable stream);
  // to flush a stream 'stream'
  #include <stdio.h>
  
*/

#include "common_hdrs.h"

#define NSECS  250000000L    /* 250,000,000 nsecs = 1/4 second */

int main(int argc, char* argv[])
{
    struct timespec sleeptime = {0,NSECS};
    int i, retval;
    int delay = 5;

    if ( argc >= 2) {
        retval = get_int(argv[1], NON_NEG_ONLY|PURE, &delay, NULL );
        if ( retval < 0 || delay < 1 )
            fatal_error(retval, "get_int expects a positive integer");
    }
    printf("Delaying for %d seconds...\n", delay);

    /* The loop iterates M times, where M = delay*(number of nanosleep
       intervals. */
    for ( i = 0; i < delay*(1000000000/NSECS); i++) {
        nanosleep(&sleeptime, NULL);
        if ( i & 1 ) printf("\r:-)");
        else         printf("\r:-(");
        fflush(stdout);
    }
    printf("Done.\n");
    return 0;
}
