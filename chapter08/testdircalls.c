#include <stdio.h>
#include <stdlib.h>

// Error handling function for fatal errors.
void fatal_error(const char* message) {
    perror(message); 
    exit(EXIT_FAILURE); 
}

int main(int argc, char* argv[]) {
  FILE *fp;
  char buff[2];

  if (argc < 2) {
    printf("Usage: %s <file-path>\n", argv[0]);
    return EXIT_FAILURE;
  }

  //  open file for reading only
  fp = fopen(argv[1], "r");
  if (fr == NULL) {
    fatal_error("Error opening file");
  }

  // attempt to read single bytes of file
  if (fread(nuf, 1, 1, fp) != 1) {
    if (feof(fp)) {
      printf("Reached end of file before reading any data.\n");
    } else {
      printf("Read successful!\n");
    }
  }
  
  // close file
  if (fclose(fp) != 0) {
    fatal_error("Error closing file\n");
  }
  return EXIT_SUCCESS;
}
