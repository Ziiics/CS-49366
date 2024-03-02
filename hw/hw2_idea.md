# Idea

## What: datelist cli
  datelist -- print future dates/times on a regular schedule
  $ datelist [-c <count>] <schedule>

  <schedule> = <num> <time-unit> [<num> <time-unit> ... ]
  <num> = [1-9][0-9]...
  <time-unit> = year[s] month[s] week[s] day[s] hour[s] minute[s]

  

Today: Mar 2 2024
user input: datelist "1 day"
output: 
Mar 4 2024
Mar 5 2024
Mar 6 2024
Mar 7 2024
Mar 8 2024
Mar 9 2024
Mar 10 2024
Mar 11 2024
Mar 12 2024
Mar 13 2024

```c
  // ### Step 1: Get todays date
  time_t     now;
  struct tm  ts;
  char       buf[80];
  struct tm* date = localtime(time(NULL));   

  strftime(buf, sizeof(buf), "%d", &date);
  printf("%s\n", buf);    
  
  // Get next 10 days from today
  
```
 