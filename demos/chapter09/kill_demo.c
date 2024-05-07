/*****************************************************************************
  Description : Sends a SIGINT then a SIGTERM to specified process
  Purpose     : A tool to help show how signal handlers work
  Usage       : Start up signal_demo3, capture its PID, and run
                        kill_demo <pid>
  Build with  : gcc -I../include -o kill_demo kill_demo.c -L../lib -lspl
*****************************************************************************/

/*

int get_int(char *arg, int flags, int *value, char *msg)
  // process a string input 'arg' and attempt to convert it to an integer based on 'flags'
  char* arg [IN] – input string to parse, provided by caller
  int flags [IN] - specify how to handle anomalies during parsing (allowed condition of arg)
  int* value [OUT] - pointer to an integer where the function will store the parsed number if successful
  char* msg [OUT] - pointer to a character array where an error message will be stored if parsing fails

int kill(pid_t pid, int sig);
  // send any signal to any process group or process
  #include <signal.h>
  pid_t pid - target process identifier (PID) to be sent, based on the value below, signal will be send to:
    > 0   : the process with the specified PID.
    == 0  : all processes in the process group of the calling process.
    < -1  : all processes in the process group whose ID is the absolute value of pid.
    == -1 : all processes for which the calling process has permission to send signals, except for process 1 (init).
  int sig - signal number to be sent, some common ones are:
    SIGTERM (15): politely asking to terminate a process
    SIGKILL (9): temrinate process immediately, CAN'T BE IGNORED
    SIGSTOP (19): pause process, CAN'T BE IGNORED
    SIGCONT (18): resume paused process
    SIGALRM (15): timer signal from alarm()

unsigned sleep(unsigned seconds);
  // cause the calling thread to be suspended from execution until time elapsed
  #include <unistd.h>

*/

#include  "common_hdrs.h"
#include  <signal.h>

int main(int argc, char* argv[])
{
    int	res, pid;
    char message[128];
    if ( argc < 2 )
        usage_error("kill_demo1 <PID of a process to signal>");

    if ( VALID_NUMBER != (res = get_int(argv[1], NO_TRAILING, &pid, message)))
        fatal_error(res, message);

    printf("%s sending SIGINT to  process %d.\n", argv[0], pid);
    if ( -1 == kill(pid, SIGINT))
        fatal_error(errno, "kill() sending SIGINT");
    sleep(1); /* Give a chance for signal to be sent. */
    printf("%s sending SIGTERM to terminate process %d.\n", argv[0], pid);
    if ( -1 == kill(pid, SIGTERM))
         fatal_error(errno, "kill() sending SIGTERM");

    return 0;
}


