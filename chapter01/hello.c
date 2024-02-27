#include <stdio.h>

int main() {
  char username[256];
  printf("Enter your name: ");
  scanf("%255s", username);
  printf("Hello, %s\n", username);
}