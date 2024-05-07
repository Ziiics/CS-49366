/******************************************************************************
  Description    : Installs signal handlers for ^C and ^\
  Purpose        : To show how to install signals and the effect of the handlers
  Usage          : signal_demo1
                   While it is running, enter ^C and ^\ a few times.
  Build with     : gcc -Wall -g -o signal_demo1
*****************************************************************************/

/*

usleep(3)
int usleep(useconds_t usec);
  /// suspends the execution of calling thread for 'usec' microseconds
  #include <unistd.h>
  
*/

#include  "common_hdrs.h"
#include  <signal.h>

int main(int argc, char* argv[])
{
    int	res, pid, i;
    char message[128];
    if ( argc < 2 )
        usage_error("kill_demo <PID of a process to signal>");

    if ( VALID_NUMBER != (res = get_int(argv[1], NO_TRAILING, &pid, message)))
        fatal_error(res, message);

    for ( i = 1; i < 100 ; i++ ) {
        usleep(100000);
        if ( -1 == kill(pid, SIGINT))
            fatal_error(errno, "kill() sending SIGINT");
    }
    printf("Sent %d SIGINT signals to %d\n", i, pid);


    return 0;
}
