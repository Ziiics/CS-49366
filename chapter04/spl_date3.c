#define _GNU_SOURCE
#include "common_hdrs.h"
#include <stdlib.h>
#include <libgen.h>
#include <time.h>

#define FORMAT "%c"           /* Default format string */
#define MAXLEN STRING_MAX     /* Maximum size of message string */
#define BAD_FORMAT_ERROR -1   /* In case user supplied bad format */
#define TIME_ADJUST_ERROR -2  /* Error to return if parsing problem */

int  get_int(char *arg, int flags, int *value, char *msg )
{
  long val;
  int res =  get_long(arg, flags, &val, msg );

  if ( VALID_NUMBER == res ) {
    if ( val > INT_MAX  || val < INT_MIN ) {
      sprintf(msg, "%ld is out of range\n", val);
      return OUT_OF_RANGE;
    } 
    else {
      *value = val;
      return VALID_NUMBER;
      }
    }
  else { /* get_long failed in one way or another */
    return res;
  }
}

int parse_time_adjustment(char* time_adjust_string, struct tm* datetm) {

char* delim = " \t";      /* Space and tab */
char* token;              /* Returned token */
int number;               /* To store number token */
char err_msg[STRING_MAX]; /* For error messages */
int flags = ONLY_DIGITS | NO_TRAILING;
int res;                  /* Return value of get_int() */

token = strtok(time_adjust_string, delim);
  while ( token != NULL ) {
    res = get_int(token, flags, &number, err_msg ); /* Get number */

    /* If error, handle it. */
    if (VALID_NUMBER != res) 
      fatal_error(res, err_msg);

    /* Number is quantity of time-adjustment unit to be found next
      get next token in time adjustment, should be a time unit. */
    token = strtok(NULL, delim); /* Get time unit, such as year, month. */

    /* If error, handle it. */
    if ( NULL == token )
      fatal_error(TIME_ADJUST_ERROR, "missing a time unit\n");

    if ( NULL != strstr(token, "year"))         datetm->tm_year += number;
    else if ( NULL != strstr(token, "month"))   datetm->tm_mon += number;
    else if ( NULL != strstr(token, "week"))    datetm->tm_mday += 7*number;
    else if ( NULL != strstr(token, "day"))     datetm->tm_mday += number;
    else if ( NULL != strstr(token, "hour"))    datetm->tm_hour += number;
    else if ( NULL != strstr(token, "minute"))  datetm->tm_min += number;
    else if ( NULL != strstr(token, "second"))  datetm->tm_sec += number;
    else
      fatal_error(TIME_ADJUST_ERROR, "Found invalid time time_unit in amount to adjust the time\n");

    /* Add number of time units to appropriate member of datetm . */
    token = strtok(NULL, delim); /* Try to get the next number. */
  }
  return 0;
}


int adjust_time(struct tm* datetm, struct tm* time_to_add) {
  datetm->tm_year += time_to_add->tm_year;
  datetm->tm_mon += time_to_add->tm_mon;
  datetm->tm_mday += time_to_add->tm_mday;
  datetm->tm_hour += time_to_add->tm_hour;
  datetm->tm_min += time_to_add->tm_min;
  datetm->tm_sec += time_to_add->tm_sec;

  errno = 0;
  mktime(datetm);
  if (errno != 0)
    fatal_error(errno, NULL);
  return 0;
}


int main(int argc, char* argv[]) {

  char formatted_date[MAXLEN];    /* String storing formatted date */
  time_t current_time;            /* Timeval in seconds since Epoch */
  struct tm *bdtime;              /* Broken-down time */
  struct tm time_adjustment= {0}; /* Broken-down time for adjustment */
  char format_string[MAXLEN];     /* String storing format spec */
  char usage_msg[512];            /* Usage message */
  char ch;                        /* For option handling */
  char options[] = ":d:h";        /* Getopt string */
  BOOL d_option = FALSE;          /* Flag to indicate -d found */
  char *d_arg;                    /* Dynamic string for -d argument */
  int d_arg_length;               /* Length of -d argument string */

  /* TUrn off error messages by getopt()*/
  opterr = 0;

  while (TRUE) {
    ch = getopt(argc, argv, options);
    if (-1 == ch) break;

    switch (ch) {

      case 'd':
        d_option = TRUE;
        d_arg_length = strlen(optarg);
        if ( NULL == d_arg )
          fatal_error(EXIT_FAILURE, "calloc could not allocate memory\n");
        strcpy(d_arg, optarg);
        break;
      
      case 'h':
        sprintf(usage_msg, "%s [-d <time adjustment>] [+\"format specification\"]", basename(argv[0]));
        usage_error(usage_msg);

      case '?' :
        fprintf(stderr,"Found invalid option %c\n", optopt);
        sprintf(usage_msg, "%s [-d <time adjustment>] [+\"format specification\"]", basename(argv[1]));
        usage_error(usage_msg);

      case ':' :
        fprintf(stderr,"Missing required argument to -d\n");
        sprintf(usage_msg, "%s [-d <time adjustment>] [+\"format specification\"]", basename(argv[0]));
        usage_error(usage_msg);
    }
  }


  if (optind < argc) {
    if ( argv[optind][0] == '+' ) /* Argument starts with + */
      strncpy(format_string, argv[optind]+1, MAXLEN-1);
    else {
      sprintf(usage_msg, "%s [-d <time adjustment>] [+\"format specification\"]\n", basename(argv[0]));
      usage_error(usage_msg);
    }
  }
  else /* No argument - use default */
    strcpy(format_string, FORMAT);

  current_time = time(NULL);
  bdtime = localtime(&current_time);

  if (bdtime == NULL)
    fatal_error(EOVERFLOW, "localtime");
  
  if (d_option) {
    parse_time_adjustment(d_arg, &time_adjustment );
    adjust_time(bdtime, &time_adjustment);
    free(d_arg); /* free memory space pointed by pointer */
  }

  if (0 == strftime(formatted_date, sizeof(formatted_date), format_string, bdtime))
    fatal_error(BAD_FORMAT_ERROR, "Conversion to a date-time string failed or produced an empty string\n");

  printf("%s\n", formatted_date);
  return 0;
}