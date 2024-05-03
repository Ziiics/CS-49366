// A code where it will not be doing nothing unless a signal comes
// We will be using sigwait()
// It will only be waiting for the signal that you put in the mask sigwait(&mask, &sig)

// sigwait() removing it from pending signal from the process, so we don't put the signal handlers in. 
// because once sigwait got it, it removes it

// sigwait is higher predecendant compared to signal handler, but not to default ahndler
// if sigwait was implemented but signal handler is not turned off, then signal handler will still run
// REMEBER! sigwait only take care of the signal WHEN its PENDING



#define _GNU_SOURCE
#include "common_hdrs.h"
#include <signal.h>

void  handler(int signum)
{
    printf("Exiting because of SIGQUIT\n");
    raise(SIGTERM);
}


int main(void)
{
    struct sigaction  sigact;
    sigset_t mask;
    int sig;

    sigact.sa_handler = handler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags  = 0;
    sigaction(SIGQUIT, &sigact, NULL);
    // sigaction(SIGINT, &sigact, NULL);
    printf("PID=%d\n", getpid());

    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGHUP);
    sigaddset(&mask, SIGUSR1);
    if ( -1 == sigprocmask(SIG_BLOCK, &mask, NULL) )
        fatal_error(errno, "sigprocmask");

    while (TRUE) {
        if ( sigwait(&mask, &sig) != 0 )      // if it is a vaild signal, it return 0
             fatal_error(errno, "sigwait");
        switch ( sig ) {
        case SIGINT: printf("SIGINT received\n"); break;
        case SIGHUP: printf("SIGHUP received\n"); break;
        default:
             printf("Received some other signal.\n");
        }
    }
    return 0;
}
