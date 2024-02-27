#include <stdlib.h>
#include <stdio.h>

void main() {
    char* num_columns = getenv("COLUMNS");
    printf("This window has %s columns.\n", num_columns);
}