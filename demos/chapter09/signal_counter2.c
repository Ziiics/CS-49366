/*****************************************************************************
  Description : Counts and prints SIGINTs delivered a different way
  Purpose     : To show how to share data between handler and rest of program
  Usage       : signal_counter2
                Enter CTRL-Cs and observe that they're counted correctly.
  Build with  : gcc -I../include -o signal_counter2 signal_counter2.c \
                     -L../lib -lspl
*****************************************************************************/

/*

int sigaddset(sigset_t *set, int signo);
  // add a signal to signal set, have to sigemptyset() or sigfillset() at least once prior to any other use of that object
  #include <signal.h>
  sigset_t *set - the set fo signal to be deleted from
  int signo - signal number from the set

int sigprocmask(int how, const sigset_t *_Nullable restrict set, sigset_t *_Nullable restrict oldset);
  // examine and change the signal mask of the calling thread
  #include <signal.h>
  int how - how the signal mask should be changed, the value options are
    SIG_BLOCK - signals specified in 'set' are added to the current signal mask
    SIG_UNBLOCK - signals specified in 'set' are removed from the current signal mask
    SIG_SETMASK - signals specified in 'set' becomes the new signal mask replacing the current mask
  const sigset_t *_Nullable restrict set - set of signals to be updated
  sigset_t *_Nullable restrict oldset - pointer to a sigset_t 
    if its NULL, the previous value of the mask won't be stored 
    if not NULL, signal mask is unchanged and 'how' is ignored
  
  
*/

#include  "common_hdrs.h"
#include  <signal.h>

static volatile sig_atomic_t count = 0;

void catch_sigint(int signum)
{
    count++;
}

int main (int argc, char* argv[])
{
    sigset_t blockedset;
    int i;
    int prev_count = 0;
    /* Initialize the signal mask. */
    sigemptyset (&blockedset);
    sigaddset (&blockedset, SIGINT);

    if ( SIG_ERR == signal(SIGINT, catch_sigint) )
        fatal_error(errno, "signal()");

    while (TRUE)    {
        /* Block SIGINT while we print the count. */
        if ( -1 == sigprocmask (SIG_BLOCK, &blockedset, NULL) )
            fatal_error(errno, "signal()");

        printf("\n%d more SIGINTs received\n", count-prev_count);
        prev_count = count;
        /* Unblock SIGINT, allowing handler to run. */
        if ( -1 == sigprocmask (SIG_UNBLOCK, &blockedset, NULL) )
            fatal_error(errno, "signal()");

        /* Delay to allow user to enter more CTRL-Cs.    */
        for ( i = 0; i < 100; i++) usleep(20000);
    }
}
