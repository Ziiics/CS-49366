/*****************************************************************************
  Description/   : Allows various flags to be passed
*****************************************************************************/

/*

sigset_t
  // a type representing a signal set, used by signal blocking functions to specify which signals are affected.
  // because blocked signal has occured but is still pending, 
  // sigset_t passed to sigpending containes the pending signals for the calling process

const char *sigabbrev_np(int sig);
  // returns abbreviated name of signal sig

action.sa_sigaction = sig_handler;       /* SIGINT handler, to specify signal catching function  */
action.sa_flags = SA_SIGINFO | flags;    /* Add the entered flags, to modifies behavior */
sigemptyset(&blocked);                   /* Clear all bits of mask.   */
action.sa_mask = blocked;                /* mask of signal to be blocked during execution of signal handler */
  // In the sigaction structure, the sa_flags field can be used to modify the behavior of a specified signal. 
  // When the SA_SIGINFO flag is set in the sa_flags field, the sa_sigaction field specifies a signal-catching function

void* memset (void* ptr, int value, size_t num);
  // sets sets the first 'num' bytes of the block of memory pointer by 'ptr' to the specified 'value'
  void* ptr - pointer to the block of memory to fill
  int value - value to be set
  size_t num - num of bytes to send to value

*/


#define _GNU_SOURCE
#include  "common_hdrs.h"
#include  <signal.h>
#include  <termios.h>    /* Needed for tcflush(). */
#define INPUTLEN  12

void sig_handler (int signo, siginfo_t *info, void *context)
{
    int      call_id;   /* Number to uniquely identify signal handler run. */
    int      i, j = 0;
    struct timespec t;  /* To store time that handler is entered. */

    /* Get current time in nanoseconds. */
    if ( -1 == clock_gettime(CLOCK_REALTIME, &t) )
        raise(SIGTERM);                /* Force termination if this failed. */
    /* Create the ID to uniquely identify this call to handler. */
    call_id = (t.tv_sec & 0xFFFF)*1000 + (t.tv_nsec / 1000000 );
    printf("Entered handler for SIG%s ID=%d\n",
           sigabbrev_np(info->si_signo), call_id );
    /* Artificially delay handler to allow time for signals to arrive. */
    for ( i = 0; i < 200000000; i++ ) { j++ ; }
    printf("Leaving handler for SIG%s ID=%d\n",
           sigabbrev_np(info->si_signo), call_id );
}

int main(int argc, char* argv[])
{
    const  int  maxsize = INPUTLEN; /* Maximum input size */
    const char  intr_message[] = "    read() was interrupted.\n";
    const char  out_label[]    = "Entered text:";
    char        buffer[maxsize+2]; /* INPUTLEN plus newline and null byte */
    struct sigaction action;
    sigset_t    blocked;             /* Set of blocked sigs */
    int         flags = 0;
    int         n, i = 1;
    char        prompt[128];
    int         reply_len = strlen(out_label);
    int         intr_message_len = strlen(intr_message);
    int         prompt_len;

    sprintf(prompt, "Type at most %d characters, then <ENTER>"
                              "(or 'quit' to quit):", maxsize);
    prompt_len = strlen(prompt);

    /* Get command line arguments and check which ones user entered. */
    while ( i < argc ) {
        if (0 == strncmp("reset", argv[i], strlen(argv[i])) )
            flags |= SA_RESETHAND;
        else if (0 == strncmp("nodefer", argv[i], strlen(argv[i])) )
            flags |= SA_NODEFER;
        else if (0 == strncmp("restart", argv[i], strlen(argv[i])) )
            flags |= SA_RESTART;
        i++;
    }

    action.sa_sigaction = sig_handler;       /* SIGINT handler            */
    action.sa_flags = SA_SIGINFO | flags;    /* Add the entered flags.    */
    sigemptyset(&blocked);                   /* Clear all bits of mask.   */
    action.sa_mask = blocked;                /* Set blocked mask.         */

    /* Install sig_handler as the SIGINT handler */
    if ( sigaction(SIGINT, &action, NULL) == -1 )
        fatal_error(errno, "sigaction");

    while( TRUE ) {
        memset((void*)buffer, 0, maxsize+2);     /* Zero inout buffer.     */
        tcflush(STDIN_FILENO,TCIFLUSH);          /* Remove bytes never sent.*/
        write(STDOUT_FILENO, prompt, prompt_len); /* Write prompt string.   */
        n = read(STDIN_FILENO, &buffer, maxsize); /* Read user input.       */
        if (-1 == n  &&  EINTR == errno )       /* If interrupted by signal */
            write(STDOUT_FILENO, intr_message, intr_message_len);
        else {
            if ( strncmp("quit", buffer, 4) == 0 ) /* User wants to quit. */
                break;
            else { /* Write the entered characters to the terminal. */
                write(STDOUT_FILENO, &out_label, reply_len);
                if ( n >= maxsize ) /* In this case, need to add a newline. */
                    buffer[n] = '\n';
                write(STDOUT_FILENO,&buffer,n+1);
            }
        }
    }
    return 0;
}
