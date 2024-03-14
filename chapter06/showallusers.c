#define _GNU_SOURCE
#define _XOPEN_SOURCE 500
#include "../common_hdrs.h"
#include <pwd.h>
int main(int argc, char *argv[])
{
  struct passwd *psswd_struct; /* Stores returned record. */
  setpwent();                  /* Initialize the iterator. */
  errno = 0;                   /* Set errno to 0 to detect error from getpwent(). */
  /* Repeatedly call getpwent() until it returns NULL. */
  while ((psswd_struct = getpwent()) != NULL)
  {
    /* Print the pw_name member of the struct. */
    printf("%s\n", psswd_struct->pw_name);
    errno = 0;
  }
  if (errno != 0)
  {
    fatal_error(errno, "getpwent");
    endpwent(); /* Close the passwd database. */
    return 0;
  }
}