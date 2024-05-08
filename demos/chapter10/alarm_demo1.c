/*****************************************************************************
  Description : Calls alarm, pause using the sigaction() to install handler
  Purpose     :
  Usage       : alarmdemo2  <numseconds>
                 where <numseconds> is the number of seconds with which to
                       set the alarm
  Build with  : gcc -Wall -g -I../include -L../lib -o alarm_demo1  \
                  alarm_demo1.c -lspl
*****************************************************************************/

#include "common_hdrs.h"
#include <signal.h>


int main(int argc, char * argv[])
{
    int    k, resultcode;

    if (2 > argc) {
        usage_error("alarm_demo1 <alarm-interval>");
    }

    resultcode = get_int(argv[1], NON_NEG_ONLY|PURE, &k, NULL ); // convert user's arg into int
    if ( resultcode < 0 || k < 1 )
        fatal_error(resultcode, "get_int expects a positive integer");

    printf( "Sleeping for %d seconds.\n",  k);
    alarm(k);
    pause();
    return 0;
}
