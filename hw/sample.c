#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {  
//   time_t     now;
//   struct tm  ts;
//   char       buf[80];

//   // Get current time
//   time(&now);
 
//   ts = *localtime(&now);
//   print(&ts);  
//   strftime(buf, sizeof(buf), "%d", &ts);

//   printf("%s\n", buf);   
  time_t     now2;
  struct tm  ts2;
  char       buf2[80];
  int varSeconds;
  int varMinutes;
  int varHours;
  int varDays;
  int varWeeks;
  int varYears;

  // switch 

  // Get current time
  time(&now2);
  now2 += 60 * 60 * 24 * 10;

  // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
  ts2 = *localtime(&now2);
  strftime(buf2, sizeof(buf2), "%a %Y-%m-%d %H:%M:%S %Z", &ts2);
  printf("%s\n", buf2);  

 
  return 0;
}
