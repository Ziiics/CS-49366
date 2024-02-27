#include <gnu/libc-version.h>   // can't use it in mac
#include <stdio.h>


int main() {
    printf("Ther verison of glibc is $s\n", gnu_get_libc_version());
    return 0;
}