// get the process id from the process that invokes it
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    printf("The process id of this process is %d.\n",getpid());
}