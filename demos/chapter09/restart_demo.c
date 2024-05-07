/*****************************************************************************
  Description : Waits for user input and a keyboard interrupt
  Purpose     : To demonstrate what happens when system calls are or are not
                restarted when they get interrupted.
  Usage       : restart_demo
  Build with  : gcc -o restart_demo restart_demo.c
  Notes          :
  To use this program:
    1. Run it and type no keyboard interrupts. It expects you to enter 4
       characters followed by Enter.  It will produce no output.
    2. Type less than 4 characters then Ctrl-C. It will output the message
       "Read call was interrupted." followed by the unaltered value XXXX.
    3. Type less than 4 characters then Ctrl-\. It will wait for you to
    type 4 more characters, followed by Enter. It will output the message
    "Read call was interrupted and restarted." followed by the last 4
    characters that you typed.
*****************************************************************************/

/*

usleep(3)
int usleep(useconds_t usec);
  /// suspends the execution of calling thread for 'usec' microseconds
  #include <unistd.h>

volatile sig_atomic_t got_interrupt= 0;
  // handle changes that occur outside the normal execution flow of a program
  // it is volatile bcs the program can always get interrupt
  volatile - used to tell compiler that the value of a variable can change anytime
  sig_atomic_t - int type that is guaranteed atomic completed in single step
  got_interrupt - a flag to indicate if there's signal coming and interrupt the running program

ssize_t write(int fildes, const void *buf, size_t nbyte);
  // write nbyte bytes from the buffer pointer to by buf to the file fildes
  // returns the number of bytes that weree actually written
  #include <unistd.h>

struct sigaction sigact - declares struct sigaction variable to specify how signals are handled

struct sigaction {
  void     (*sa_handler)(int);
  void     (*sa_sigaction)(int, siginfo_t *, void *);
  igset_t   sa_mask;
  int        sa_flags;
  void     (*sa_restorer)(void);
};

sigact.sa_handler = on_interrupt; // caught a signal
sigemptyset(&sigact.sa_mask); // furthur signal that is same with the current handled signal will not be blocked until completion
sigact.sa_flags = 0; // not using special signal handling which means syscalls won't start automatically


int sigaction(int sig, const struct sigaction *restrict act, struct sigaction *restrict oact);
  // examine and change a signal action
  #include <signal.h>

int tcflush(int fd, int queue_selector);
  #include <termios.h>
  #include <unistd.h>
  int sig - specifies the signal number which action will be change
  const struct sigaction *restrict act - pointer to a struct sigaction structure that defines the new action
    Contains several fields:
      - sa_handler or sa_sigaction: Specifies the handler function or action when the signal is received.
      - sa_mask: A set of signals to be blocked during the execution of the signal handling function.
      - sa_flags: Flags that modify the behavior of the signal handling; 
          ex: SA_RESTART to make certain interrupted system calls restart automatically.
  struct sigaction *restrict oact - OPTIONAL, If not null, sigaction() will save the previous action of specified signal sig in the oact

*/

#include "common_hdrs.h"
#include <signal.h>
#include <termios.h>

volatile sig_atomic_t  got_interrupt= 0;
char alert[] = "\nSignal caught; re-enter input.\n> ";
int  alertlen;

void on_interrupt(int signo)
{
    got_interrupt = 1;
    if ( SIGQUIT == signo )
        write(1, alert, alertlen);
}

int main(int argc, char* argv[])
{
    struct sigaction sigact;
    int n;
    char c[5] = "XXXX"; // bcs last is temrinator

    alertlen = strlen(alert);
    sigact.sa_handler = on_interrupt; // caught a signal
    sigemptyset(&sigact.sa_mask);     // dont care about the other signal
    sigact.sa_flags = 0;              // do nothing after done - actually have more selection

    if (-1 == sigaction(SIGINT, &sigact, NULL))
        fatal_error(errno, "sigaction()");

    sigact.sa_flags = SA_RESTART;
    if (sigaction(SIGQUIT, &sigact, NULL))
        fatal_error(errno, "sigaction()");

    /* Request a read of 4 chars. */
    n = read(0, &c, 4);

    /* If a SIGINT interrupt was received, SA_RESTART was not set
       so the call will not be restarted. */
    if (-1 == n  &&  EINTR == errno ) {
        printf("Read call was interrupted.\n");
        write(1, c, 4);
        printf("\n");
    }
    /* If a SIGQUIT interrupt was received SA_RESTART was on
       so the call will be restarted. */
    else if (got_interrupt) {
        printf("Read call was interrupted and restarted.\n");
        write(1, c, 4);
        printf("\n");
    }
    else
        printf("You entered %s\n", c);

    /* Drain the terminal queues in case there are characters still there. */
    tcflush(0,TCIFLUSH);
    return 0;
}

