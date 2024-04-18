#include <stdio.h>
#include <stdlib.h>

// Mimics error handling seen in your code. Adjust as needed.
void fatal_error(const char* message) {
    perror(message); // Print the last error encountered by a library function
    exit(EXIT_FAILURE); // Exit the program with an error status
}

int main(int argc, char *argv[]) {
    FILE *file;
    int i;

    if (argc == 1) {
        // No files provided as arguments; mimic listing current directory (not possible with stdlib)
        printf("Usage: %s <file1> [file2] ...\n", argv[0]);
        return EXIT_FAILURE;
    }

    for (i = 1; i < argc; i++) {
        file = fopen(argv[i], "r");
        if (file == NULL) {
            // Could not open file, mimic error handling
            printf("Cannot open file: %s\n", argv[i]);
            continue;
        }
        
        printf("Accessible file: %s\n", argv[i]);
        fclose(file);
    }

    return EXIT_SUCCESS;
}
