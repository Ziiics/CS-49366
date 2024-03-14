#define _GNU_SOURCE
#include "../common_hdrs.h"
#include <lastlog.h> /* For lastlog structure definition */
#include <paths.h>   /* For definition of _PATH_LASTLOG */
#include <pwd.h>     /* For password file iterators */
#define MESSAGE_SIZE 512
#define FORMAT "%c"     /* Default format string */
#define READ_ERROR -2   /* Error reading from lastlog file */
#define LOCALE_ERROR -3 /* Error calling setlocale() */
/* Prints a line for a username who has never logged in. */
void print_never_logged_in(char *uname)
{
  printf("%-16s %-8.8s %-16s **Never logged in**\n", uname, " ", " ");
}
int main(int argc, char *argv[])
{
  struct lastlog ll_entry;     /* To store lastlog record read from file */
  struct passwd *psswd_struct; /* passwd structure from password file */
  int ll_fd;                   /* File descriptor of lastlog file */
  off_t ll_file_size;          /* Size of lastlog file, in bytes */
  size_t ll_struct_size;       /* Size in bytes of lastlog structure */
  size_t num_bytes;            /* Number of bytes read in read() */
  uid_t uid;                   /* user ID of current search
                    char *username; /* Username of current search */
  int highest_uid;             /* Highest user ID in lastlog file */
  char lastlog_time[64];       /* Localized date/time string */
  time_t ll_time;              /* Lastlog time converted to time_t */
  struct tm *bdtime;           /* Broken-down time */
  errno = 0;
  if ((ll_fd = open(_PATH_LASTLOG, O_RDONLY)) == -1)
    fatal_error(errno, "while opening " _PATH_LASTLOG);
  ll_file_size = lseek(ll_fd, 0, SEEK_END); /* Get size of lastlog file. */
  ll_struct_size = sizeof(struct lastlog);  /* Get size of lastlog struct. */
  highest_uid = ll_file_size / ll_struct_size - 1;
  if (setlocale(LC_ALL, "") == NULL)
    fatal_error(LOCALE_ERROR, "setlocale() could not set the given locale");
  setpwent(); /* Initialize the passwd file iterator. */
  printf("Username Port From Last Login\n");
  while ((psswd_struct = getpwent()) != NULL)
  {
    uid = psswd_struct->pw_uid;
    username = psswd_struct->pw_name;
    if (uid > highest_uid)
      print_never_logged_in(username);
    else
    {
      if (lseek(ll_fd, uid * ll_struct_size, SEEK_SET) == -1)
        fatal_error(errno, "lseek");
      errno = 0;
      if ((num_bytes = read(ll_fd, &ll_entry, ll_struct_size)) <= 0)
      {
        if (0 != errno) /* A read error occurred */
          fatal_error(errno, "read");
        else
        { /* Not a read error - shouldn't happen but continue */
          error_mssge(-1, "could not read the entry, skipping");
          continue;
        }
      }
      else if (num_bytes != ll_struct_size)
        fatal_error(READ_ERROR, "incomplete read of lastlog struct");
      if (0 == ll_entry.ll_time) /* No entry for this user */
        print_never_logged_in(username);
      else
      {
/* Convert the lastlog time into broken-down time. */
#if __WORDSIZE_TIME64_COMPAT32
        ll_time = ll_entry.ll_time;
        bdtime = localtime(&ll_time);
#else
        bdtime = localtime(&(ll_entry.ll_time));
#endif
        if (bdtime == NULL) /* The only possible error is EOVERFLOW. */
          fatal_error(EOVERFLOW, "localtime");
        if (0 == strftime(lastlog_time, sizeof(lastlog_time),
                          FORMAT, bdtime))
          fatal_error(-1, "Conversion to a date-time string failed "
                          " or produced an empty string\n");
        printf("%-16s %-8.8s %-16s %s\n", username, ll_entry.ll_line,
               ll_entry.ll_host, lastlog_time);
      }
    }
  }
  close(ll_fd);
  exit(EXIT_SUCCESS);
}