/*****************************************************************************
  Description : Calls alarm, pause using the sigaction() to install handler
  Purpose     :
  Usage       : alarmdemo3  <numseconds>
                 where <numseconds> is the number of seconds with which to
                       set the alarm
  Build with  : gcc -Wall -g -I../include -L../lib -o alarm_demo3  \
                  alarm_demo3.c -lspl
*****************************************************************************/

/*

size_t strftime(char *s, size_t max, const char *format, const struct tm *tm);
  // format the time represneted by 'struct tm' into a string according to a specified format
  #include <time.h>
  char *s - pointer to char array where the formatted string will be stored
  size_t max - max num of char to be written to 's', including it's null terminator
  const char *format - format of the outputted string (as time), as %A, %d, etc
  const struct tm *tm - pointer to 'struct tm' containing the time info to be formatted

int sigfillset(sigset_t *set);  
  // initializes set to full, to include all the signals (make it containes all the possible signals)

struct tm *localtime(const time_t *timep);
  // convert time represented by 'time_t' object into struct tm representing local time

*/


#include "common_hdrs.h"
#include <signal.h>



/* This signal handler catches the SIGALRM and prints its delivery time.    */
void catchalarm(int signo )
{
    time_t t;
    struct tm *tp;
    char   timestr[100];
    t = time(NULL);
    if ( NULL == (tp = localtime(&t)))
        fatal_error(-1, "localtime");
    strftime(timestr, sizeof(timestr), "Alarm received at %T\n", tp);
    printf("%s", timestr );
}

int main(int argc, char * argv[])
{
    int    k;
    struct tm *bdtime;
    time_t t;
    struct sigaction act;
    char   timestr[100];
    int    resultcode;

    if (2 > argc) {
        usage_error("alarm_demo3 <alarm-interval>");
    }

    resultcode = get_int(argv[1], NON_NEG_ONLY|PURE, &k, NULL );
    if ( resultcode < 0 || k < 1 )
        fatal_error(resultcode, "get_int expects a positive integer");

    act.sa_handler = catchalarm;    /* install catchalarm as handler */
    sigfillset (&(act.sa_mask));    /* create a full mask */
    sigaction(SIGALRM, &act, NULL); /* catchalarm handles SIGALRM */

    time (&t);
    if ( (bdtime = localtime(&t)) == NULL )
        fatal_error(EOVERFLOW, "localtime");

    if (0 == strftime(timestr, sizeof(timestr), "The time is now %T", bdtime))
        fatal_error(BAD_FORMAT_ERROR,
                     "Conversion to a date-time string failed\n");
    printf( "%s. About to set an alarm for %d seconds.\n", timestr, k);
    alarm(k);
    pause();  // suspend the execution of the program until signal is received, wiaitng for SIGALARM
    return 0;
}
