/* What I need:
  - Get the input from the user
  - case -c, output amount of line based on count
    else do 10 line
  - if schedule < day, output also time 
    else only day
*/

/* The function i need:
  - adjust time and add it to schedule
  - parse time adjustment, take input argument and output struct *tm
*/

#define _GNU_SOURCE
#include "common_hdrs.h"
#include <stdlib.h>
#include <libgen.h>
#include <time.h> 

int get_long(char *arg, int flags, long *value, char *msg) {
  char *endptr;
  long val;
  errno = 0;    /* To distinguish success/failure after call */
  val = strtol(arg, &endptr, 0);

  /* Check for various possible errors */
  if (errno == ERANGE) {
    if (msg != NULL )
      sprintf(msg, "%s\n", strerror(errno));
    return FATAL_ERROR;
  } else if ( errno == EINVAL && val != 0 ) {
    /* bad base chars -- shouldn't happen */
    if (msg != NULL )
      sprintf(msg, "%s\n", strerror(errno));
      return FATAL_ERROR;
  }
  /* errno == 0 or errno == EINVAL && val == 0 */

  if (endptr == arg) {
    /* the first invalid char is the first char of the string */
    if ( flags & ( ONLY_DIGITS | NO_TRAILING ) ) {
      if (msg != NULL )
        sprintf(msg, "No digits in the string\n");
      return NO_DIGITS_FOUND;
    }
    else { /* accept a zero result */
      *value = 0;
      return VALID_NUMBER;
    }
  }
  /* endptr != arg but endptr is not at end of string */

  if (*endptr != '\0' && *endptr != ' ') {
    /* there are non-number characters following the number.
     which we can call an error or not, depending. */
    if ( flags & NO_TRAILING ) {
      if ( msg != NULL )
        sprintf(msg, "Trailing characters follow the number: \"%s\"\n", endptr);
      return TRAILING_CHARS_FOUND;
    }
  }

  /* If we got here, strtol() successfully parsed a number, but it might be negative, so check flag and report if necessary */
  if ( flags & NON_NEG_ONLY )
    if ( val < 0 ) {
      if (msg != NULL )
        sprintf(msg, "number is negative\n");
    arg = endptr;
    return NEG_NUM_FOUND;
  }

  *value = val;
  return VALID_NUMBER;
}


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
  char options[] = ":c";        /* Getopt string */
  BOOL c_option = FALSE;          /* Flag to indicate -c found */
  int *c_arg;                      /* int for argument d */

  /* TUrn off error messages by getopt()*/
  opterr = 0;

  while (TRUE) {
    ch = getopt(argc, argv, options);
    if (-1 == ch) break;

    switch (ch) {

      case 'c':
        c_option = TRUE;
        c_arg = malloc(strlen(optarg));
        if ( NULL == c_arg )
          fatal_error(EXIT_FAILURE, "calloc could not allocate memory\n");
        strcpy(c_arg, optarg);
        break;
      
      case 'h':
        sprintf(usage_msg, "%s [-c <count>] <schedule>", basename(argv[0]));
        usage_error(usage_msg);

      case '?' :
        fprintf(stderr,"Found invalid option %c\n", optopt);
        sprintf(usage_msg, "%s [-c <count>] <schedule>", basename(argv[0]));
        usage_error(usage_msg);

      case ':' :
        fprintf(stderr,"Missing required argument to -d\n");
        sprintf(usage_msg, "%s [-c <count>] <schedule>", basename(argv[0]));
        usage_error(usage_msg);
    }
  }


  if (optind < argc) {
    if ( argv[optind][0] != NULL ) /* Have argument */
      strncpy(format_string, argv[optind], MAXLEN-1);
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
  
  if (c_option) {
    parse_time_adjustment(c_arg, &time_adjustment );
    adjust_time(bdtime, &time_adjustment);
    free(d_arg); /* free memory space pointed by pointer */
  }

  if (0 == strftime(formatted_date, sizeof(formatted_date), format_string, bdtime))
    fatal_error(BAD_FORMAT_ERROR, "Conversion to a date-time string failed or produced an empty string\n");

  printf("%s\n", formatted_date);
  return 0;
}

