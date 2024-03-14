#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

typedef struct schedule_t
{
    char *key;
    int value;
} schedule_t;

void add_unix_in_schedule(struct tm *date, int num, char time_unit[])
{
    if (strcmp(time_unit, "years") == 0)
        date->tm_year += num;
    else if (strcmp(time_unit, "months") == 0)
        date->tm_mon += num;
    else if (strcmp(time_unit, "weeks") == 0)
        date->tm_mday += num * 7;
    else if (strcmp(time_unit, "days") == 0)
        date->tm_mday += num;
    else if (strcmp(time_unit, "hours") == 0)
        date->tm_hour += num;
    else if (strcmp(time_unit, "minutes") == 0)
        date->tm_min += num;
    else if (strcmp(time_unit, "seconds") == 0)
        date->tm_sec += num;
    else
    {
        printf("Invalid time unit: %s\n", time_unit);
        return;
    }

    // Normalize the date
    mktime(date);
}

int main(int argc, char *argv[])
{
    // make sure there are even key pair arguments
    if (argc % 2 == 0)
    {
        printf("Invalid arguments");
        return 1;
    }

    schedule_t schedule_list[] = {
        {"years", 0},
        {"months", 0},
        {"weeks", 0},
        {"days", 0},
        {"hours", 0},
        {"minutes", 0}};

    time_t currentTime;
    struct tm *currentDate;
    time(&currentTime);
    currentDate = localtime(&currentTime);
    time_t epochBefore = mktime(currentDate);
    time_t t = time(NULL);
    int schedule_count = 10;

    for (int i = 1; i < argc; i += 2)
    {
        if (strcmp(argv[i], "-c") == 0)
        {
            // set schedule_count
            schedule_count = atoi(argv[i + 1]);
        }
        else
        {
            int schedule_amt = atoi(argv[i]);
            char *schedule_val = argv[i + 1];
            bool valid_schedule_val = false;

            for (int k = 0; k < sizeof(schedule_list) / sizeof(schedule_list[0]); k++)
            {
                // Compare the key with the target string
                if (strcmp(schedule_list[k].key, schedule_val) == 0)
                {
                    valid_schedule_val = true;
                    break;
                }
            }

            if (!valid_schedule_val)
            {
                printf("Invalid arguments");
                return 0;
            }

            add_unix_in_schedule(currentDate, schedule_amt, schedule_val);
        }
    }

    struct tm time_difference;
    time_t epochAfter = mktime(currentDate);
    time_difference.tm_sec = difftime(epochAfter, epochBefore);
    for (int i = 0; i < schedule_count; i++)
    {
        printf("%d: %d-%02d-%02d %02d:%02d:%02d\n", i + 1, currentDate->tm_year + 1900, currentDate->tm_mon + 1, currentDate->tm_mday, currentDate->tm_hour, currentDate->tm_min, currentDate->tm_sec);
        add_unix_in_schedule(currentDate, time_difference.tm_sec, "seconds");
    }

    return 0;
}