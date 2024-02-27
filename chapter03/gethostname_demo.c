#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int main() {
    char name[4];
    size_t len =3;
    int returnvalue;

    returnvalue = gethostname(name, len);
    if (-1 == returnvalue) {
        switch (errno) {
            case EFAULT:
                printf("A bad address was passed for the string name\n"); break;
            case EINVAL:
                printf("The length argument was negative.\n"); break;
            case ENAMETOOLONG:
                printf("The hostname is too long for the allocated array.\n");
        }
    } else 
        printf("%s\n", name);
}