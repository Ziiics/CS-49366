#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#define MAXLEN 80

int adjust_time(char *schedule, struct tm *adjusted_time)
{

  char *token;
  token = strtok(schedule, " \t");

  printf("%s\n", token);

  while (token != NULL)
  {
    int nums = atoi(token);
    token = strtok(NULL, " \t");

    if (strstr(token, "second") != NULL)
      adjusted_time->tm_sec += nums;
    else if (strstr(token, "minutes") != NULL)
      adjusted_time->tm_min += nums;
    else if (strstr(token, "week") != NULL)
      adjusted_time->tm_mday += 7 * nums;
    else if (strstr(token, "day") != NULL)
      adjusted_time->tm_mday += nums;
    else if (strstr(token, "hour") != NULL)
      adjusted_time->tm_hour += nums;
    else if (strstr(token, "minute") != NULL)
      adjusted_time->tm_min += nums;

    else
    {
      token = strtok(NULL, " \t");
      printf("Found invalid adjust time: %s\n", token);
      break;
    }
    token = strtok(NULL, " \t");
  }
  return 0;
}

int main(int argc, char *argv[])
{

  /* naming */
  time_t today;
  struct tm *today_time;
  char str_format[MAXLEN];
  char *schedule;
  bool c_option = false;
  int ch;
  char options[] = "c:";
  int count = 10;
  struct tm adjusted_time;
  char *schedule_arg = NULL;


  /* getting current time value */
  today = time(NULL);
  today_time = localtime(&today);
  strftime(str_format, MAXLEN, "%c", today_time);


  /* Check user's command */
  if (argc < 2)
    printf("%s: format should be [-c <count>] <schedule>\n", argv[0]);
  else {
    if (strcmp(argv[1], "-c") == 0) 
      count = atoi (argv[2]);
  }
  schedule_arg = argv[argc-1];
  printf("Count: %d, Schedule: %s\n", count, schedule_arg);


  /* change the time and date */


  /* iterate */


  /* outputting part */
  printf("%s\n", str_format);
  
  return 0;
}