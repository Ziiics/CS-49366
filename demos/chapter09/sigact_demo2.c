/*****************************************************************************
  Description : Displays siginfo information about SIGFPE sent to it
  Purpose     : To show how to use siginfo_t info data
  Usage       : sigact_demo2  [f]
                The f argument causes a floating point error; without it,
                it will have a divide-by-zero error.
  Build with  : gcc -O0 -o sigact_demo2 -I../include -L../lib sigact_demo2.c \
                     -lm  -lspl
*****************************************************************************/

/*

char *strsignal(int sig);
  // return string describing signal
  #include <string.h>

typedef struct {
	int si_signo;     // signal number being delivered, always set
	int si_code;      // signal code, always set (system generated)
	union sigval si_value;  // signal value
	int si_errno;     // if non-zero, errno value associated w/ this signal
	pid_t si_pid;     // pid of sending process
	uid_t si_uid;     // real uid of sending process
	void *si_addr;    // address at which fault occured
	int si_status;    // exit value or signal for process termination
	int si_band;      // band event for SIGPOLL/SIGIO
} siginfo_t;






*/

#define _GNU_SOURCE
#include "common_hdrs.h"
#include <signal.h>
#include <math.h>
#include <fenv.h>

BOOL noreturn = FALSE;

void fpe_handler (int signo, siginfo_t *info, void *context)
{
    printf ("Signal: %s\n", strsignal(info->si_signo));      /* UNSAFE */
    switch ( info->si_code ) {
        case FPE_INTDIV :
        printf("Code: FPE_INTDIV (Integer divide by zero)\n");
        break;

        case FPE_INTOVF :
        printf("Code: FPE_INTOVF (Integer overflow)\n");
        break;

        case FPE_FLTDIV :
        printf("Code: FPE_FLTDIV (Floating-point divide by zero)\n");
        break;

        case FPE_FLTOVF :
        printf("Code: FPE_FLTOVF (Floating-point overflow)\n");
        break;

        case FPE_FLTUND :
        printf("Code: FPE_FLTUND (Floating-point underflow)\n");
        break;

        case FPE_FLTRES :
        printf("Code: FPE_FLTRES (Floating-point inexact result)\n");
        break;

        case FPE_FLTINV :
        printf("Code: FPE_FLTINV (Floating-point invalid operation)\n");
        break;

        case FPE_FLTSUB :
        printf("Code: FPE_FLTSUB (Subscript out of range)\n");
        break;
    }
    if ( ! noreturn )
        raise(SIGTERM); /* To force program to quit. */
}

int main(int argc, char* argv[])
{
    struct sigaction action;
    float      y=2.0, z = 0.0;
    BOOL  float_divzero = FALSE;
    BOOL  float_overflow = FALSE;
    signed int  n=1,m=2;

    if ( 2 == argc )
        switch (argv[1][0]) {
        case 'f' : float_divzero  = TRUE; break;
        case 'o' : float_overflow = TRUE; break;
        case '0' : noreturn       = TRUE; break;
        }

    action.sa_sigaction = fpe_handler;
    action.sa_flags = SA_SIGINFO;
    sigemptyset(&(action.sa_mask));
    int excepts = FE_DIVBYZERO|FE_INEXACT|FE_INVALID|FE_OVERFLOW|FE_UNDERFLOW;
    feenableexcept(excepts);
    if ( sigaction(SIGFPE, &action, NULL) == -1 ) {
        perror("sigaction");
        exit(1);
    }


    m = 2*n - m;
    if ( float_divzero )
        n = (int) y/z;
    else if ( float_overflow )
        feraiseexcept(FE_OVERFLOW);
    else
        n = n/m;

    return n;
}
