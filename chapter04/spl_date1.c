#define _GNU_SOURCE
#include "common_hdrs.h"
#define FORMAT "%c"

int main (int argc, char* argv[]) {
  
  char formatted_date[200];
  time_t current_time;
  time_t time_now;
  struct tm *broken_down_time;

  /* Get the current time */
  current_time = time(NULL);

  /* Convert current time into broken-down time */
  broken_down_time = localtime(&current_time);

  if (broken_down_time == NULL) 
    fatal_error(EOVERFLOW, "localtime");

  /* Create a string from the broken down time using the %c format */
  if (0 == strftime(formatted_date, sizeof(formatted_date), FORMAT, broken_down_time)) {
    fatal_error(EXIT_FAILURE, "Conversion to a date-time string failed  or produced an empty string\n");
  }

  printf("%s\n", formatted_date);
  return 0;
}









