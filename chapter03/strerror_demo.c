#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int main() {
    char name[4];
    size_t len = 3;
    int returnvalue;

    returnvalue = gethostname(name, len);
    if (-1 == returnvalue) {
        printf("gethostname failed: %s\n",strerror(errno));
    } else {
        printf("%s\n", name);
    }
}