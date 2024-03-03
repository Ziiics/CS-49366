#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#define MAXLEN 80


/* change the time and date */
int adjust_time(char *schedule_arg, struct tm *adjusted_time) {
  
  char *token;
  token = strtok(schedule_arg, " ");


  while (token != NULL)
  {
    printf("%s\n", token);
    int nums = atoi(token);
    token = strtok(NULL, " \t");

    if (token == NULL) 
      printf("Time units required");

    if (strstr(token, "minutes") != NULL)
      adjusted_time->tm_min += nums;
    else if (strstr(token, "hour") != NULL)
      adjusted_time->tm_hour += nums;
    else if (strstr(token, "day") != NULL)
      adjusted_time->tm_mday += nums;
    else if (strstr(token, "week") != NULL)
      adjusted_time->tm_mday += 7*nums;
    else if (strstr(token, "month") != NULL)
      adjusted_time->tm_mon += nums;
    else if (strstr(token, "year") != NULL)
      adjusted_time->tm_year += nums;

    else
    {
      printf("Found invalid adjust time: %s\n", token);
      break;
    }
    token = strtok(NULL, " \t");
  }
  return 0;
}






/* iterate */
void iterate(struct tm* adjusted_time, int count) {
  printf("Processsing iterate\n");
  time_t currentTime = time(NULL);
  struct tm* timeInfo = localtime(&currentTime);

  printf("can i print?\n");

  for (int i=0; i<count; i++) {
    char buffer=0;

    timeInfo->tm_min += adjusted_time->tm_min;
    timeInfo->tm_hour += adjusted_time->tm_hour;
    timeInfo->tm_mday += adjusted_time->tm_mday;
    timeInfo->tm_mday += adjusted_time->tm_mday;
    timeInfo->tm_mon += adjusted_time->tm_mon;
    timeInfo->tm_year += adjusted_time->tm_year;

    mktime(timeInfo);
    strftime(&buffer, 80, "%c", timeInfo);
    printf("%s/n", buffer);
  }
}








/* main function */
int main(int argc, char *argv[])
{

  /* naming */
  time_t today;
  struct tm *today_time;
  char str_format[MAXLEN];
  char *schedule_updated;
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

  /* get value to be adjusted and iterate through it */
  adjust_time(schedule_arg, schedule_updated);


  /* iterate throught it */
  iterate(schedule_updated, count);

  /* outputting part */
  printf("%s\n", str_format);
  
  return 0;
}