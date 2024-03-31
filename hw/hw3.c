#include <stdio.h>
#include <stdlib.h>
#include <utmpx.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

struct utmp_list {
  struct utmpx ut;
  struct utmp_list *next;
  struct utmp_list *prev;
}; typedef struct utmp_list utlist;


int main(int argc, char *argv[])
{
  struct utmpx *ut;

  setutxent_wtmp(1);

  while ((ut = getutxent_wtmp()) != NULL)
  {
    if (ut->ut_type == 7 || ut->ut_type == 8)
    {
      char *time_str = ctime((const time_t *)&ut->ut_tv.tv_sec);
      time_str[strlen(time_str) - 1] = '\0';

      if (ut->ut_type == 8)
      {
        printf("%-8s %-12s %s - logout\n", ut->ut_user, ut->ut_line, time_str);
      }
      else
      {
        printf("%-8s %-12s %s\n", ut->ut_user, ut->ut_line, time_str);
      }
    }
  }

  endutxent_wtmp();

  return 0;
}


