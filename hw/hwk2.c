/******************************************************************************
  Title : hwk2.c
  Author : Zivana C Syahputri
  Created on : March 1st, 2024
  Description : Able to print time based on the interval which can be
  chosen by the user
  Purpose : Print future dates/times on a regular schedule
  Usage : datelist datelist [-c <count>] <schedule>
  <schedule> = <num> <time-unit> [<num> <time-unit> ... ]
  <num> = [1-9][0-9]...
  <time-unit> = year[s] month[s] week[s] day[s] hour[s] minute[s]
  Build with : gcc -o datelist hwk2.c
  Modifications: March 4th, 2024
  Unable time unit duplicate, change time format
******************************************************************************/

#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

typedef struct schedule_t
{
    char *key;
    char *key2;
    int value;
} schedule_t;

typedef struct
{
    int month_num;
    char *month_name;
} month;

void parse_time(struct tm *date, int num, char time_unit[])
{
    if (0 == strcmp(time_unit, "years") || 0 == strcmp(time_unit, "year"))
        date->tm_year += num;
    else if (0 == strcmp(time_unit, "months") || 0 == strcmp(time_unit, "month"))
        date->tm_mon += num;
    else if (0 == strcmp(time_unit, "weeks") || 0 == strcmp(time_unit, "week"))
        date->tm_mday += num * 7;
    else if (0 == strcmp(time_unit, "days") || 0 == strcmp(time_unit, "day"))
        date->tm_mday += num;
    else if (0 == strcmp(time_unit, "hours") || 0 == strcmp(time_unit, "hour"))
        date->tm_hour += num;
    else if (0 == strcmp(time_unit, "minutes") || 0 == strcmp(time_unit, "minute"))
        date->tm_min += num;
    else if (0 == strcmp(time_unit, "seconds"))
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
    time_t currentTime = time(NULL);                      /* Current system time */
    struct tm *current_date = localtime(&currentTime);    /* Local time from currentTime */
    time_t normalized_time_before = mktime(current_date); /* Normalized start time */
    time_t currentEpochTime = time(NULL);                 /* For current time or elapsed calculations */
    int schedule_count = 10;                              /* Number of schedules */
    struct tm time_difference;                            /* For time calculations */
    double intpart;

    schedule_t schedule_list[] = {
        {"years", "year", 0},
        {"months", "minute", 0},
        {"weeks", "week", 0},
        {"days", "day", 0},
        {"hours", "hour", 0},
        {"minutes", "minute", 0}};

    month the_month[] = {
        {1, "Jan"},
        {2, "Feb"},
        {3, "Mar"},
        {4, "Apr"},
        {5, "May"},
        {6, "Jun"},
        {7, "Jul"},
        {8, "Aug"},
        {9, "Sep"},
        {10, "Oct"},
        {11, "Nov"},
        {12, "Dec"}};

    // make sure there are even key pair arguments
    if (argc % 2 == 0)
    {
        printf("Invalid argument. \nUsage: %s [-c <count>] <schedule>\n", argv[0]);
        return 1;
    }

    /* Tracks whether a unit has been used */
    bool used_units[sizeof(schedule_list) / sizeof(schedule_list[0])] = {0};
    for (int i = 1; i < argc; i += 2)
    {
        if (strcmp(argv[i], "-c") == 0)
        {
            // set schedule_count
            schedule_count = atoi(argv[i + 1]);
            if (schedule_count < 1)
            {
                printf("Count argument have to be at least 1\n");
                printf("Invalid argument. \nUsage: %s [-c <count>] <schedule>\n", argv[0]);
                return 1;
            }
        }
        else
        {
            int schedule_amt = atoi(argv[i]);
            if (modf(schedule_amt, &intpart) != 0.0)
            {
                printf("Invalid time.\nNumber have to be integer without decimal");
                return 1;
            }
            char *schedule_val = argv[i + 1];
            bool valid_schedule_val = false;

            for (int k = 0; k < sizeof(schedule_list) / sizeof(schedule_list[0]); k++)
            {
                // Compare the key with the target string
                if (strcmp(schedule_list[k].key, schedule_val) == 0 || strcmp(schedule_list[k].key2, schedule_val) == 0)
                {
                    if (!used_units[k])
                    {
                        valid_schedule_val = true;
                        used_units[k] = true;
                        break;
                    } else {
                        printf("There is a duplicate time unit. You are not allowed to use duplicate unit!\n");
                        return 1;
                    }
                }
            }

            if (!valid_schedule_val)
            {
                printf("Invalid time unit used\n");
                return 0;
            }

            parse_time(current_date, schedule_amt, schedule_val);
        }
    }

    time_t normalized_time_after = mktime(current_date); /* Normalized time after changes */

    time_difference.tm_sec = difftime(normalized_time_after, normalized_time_before);

    for (int i = 0; i < schedule_count; i++)
    {
        if (time_difference.tm_sec < 86400)
        { // Less than a day
            printf("%s %02d, %d %02d:%02d\n",
                   the_month[current_date->tm_mon].month_name,
                   current_date->tm_mday,
                   current_date->tm_year + 1900,
                   current_date->tm_hour,
                   current_date->tm_min);
        }
        else
        {                           // A day or more
            printf("%s %02d, %d\n", // Only print the date
                   the_month[current_date->tm_mon].month_name,
                   current_date->tm_mday,
                   current_date->tm_year + 1900);
        }
        parse_time(current_date, time_difference.tm_sec, "seconds");
    }

    return 0;
}