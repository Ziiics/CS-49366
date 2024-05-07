/*****************************************************************************
  Description : Handles SIGTERM by "cleaning up" and then resuming
  Purpose     : To show one use of raise() in signal handlers
  Usage       : raise_demo
                   While it's running send CTRL-\ and CTRL-C to quit.
  Build with  : gcc -I../include -o raise_demo raise_demo.c -L../lib -lspl
*****************************************************************************/

/*

int raise(int sig);
  // sends signal 'sig' to the calling process or thread
  #include <signal.h>

sighandler_t signal(int signum, sighandler_t handler);
  // sets the disposition of the signal signum to handler
  // Avoid its use: use sigaction(2) instead
  // The signals SIGKILL and SIGSTOP cannot be caught or ignored.
  #include <signal.h>
  int signum - the signal number to handle
  sighandler_t handler - pointer to the signal handling function of the specified signal
    SIG_IGN to ignore the signal.
    SIG_DFL to reset the handling to its default behavior.


*/

#include "common_hdrs.h"
#include <signal.h>

// custom handler for SIGTSTP signal (CTRL + Z)
// change only the first action, the next will time default stop action will take place
void sigtstp_handler(int signum)
{
    if ( SIG_ERR == signal( SIGTSTP, SIG_DFL ) )
        fatal_error(errno, "signal()");
    printf("\ncleaning up in progress...\ndone\n");
    raise( SIGTSTP );
    printf("raise() called to stop process.\n");  // should not appear bcs process stopped by raise
}

// When the process is resumed, reset the sigtstp handler so that it
// cleans up again before stopping
void sigcont_handler(int signum)
{
    
    if ( SIG_ERR == signal(SIGTSTP, sigtstp_handler) )
        fatal_error(errno, "signal()");
}

int main()
{
    int	i;
    if ( SIG_ERR == signal( SIGTSTP, sigtstp_handler ) )
        fatal_error(errno, "signal()");
    if ( SIG_ERR == signal( SIGCONT, sigcont_handler ) )
        fatal_error(errno, "signal()");
    for( i = 20; i > 0; i-- ) {
        printf("Enter CTRL-Z to stop the process, or CTRL-C to end it.\n");
        sleep(4);
    }
    return 0;
}



