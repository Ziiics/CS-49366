// obtaining thread id
#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    printf("The thread id number is %d.\n", gettid());
}