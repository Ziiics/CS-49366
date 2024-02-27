#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    char* endptr;
    long val;
    if (argc < 2) {
        fprintf(stderr, "Usage %s str\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    errno = 0;
    val = strtol(argv[1], &endptr, 0);

    if (errno != 0) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }
    if (endptr == argv[1]) {
        fprintf(stderr, "No digits were found\n");
        exit(EXIT_FAILURE);
    }
    if (*endptr != '\0') {
        printf("Characters following the number: \"%s\"\n", endptr);
    }

    printf("strtol() returned %ld\n", val);
    exit(EXIT_FAILURE);
}