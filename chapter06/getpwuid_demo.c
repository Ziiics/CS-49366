#include "../common_hdrs.h"
#include <pwd.h>

int main(int argc, char *argv[])
{
  uid_t userid;

  struct passwd *psswd_struct; /* To save pointer returned by getpwuid() */

  /* Get the real user ID associated with the process,
  which is the same as that of the user who runs this command. */
  userid = getuid();

  /* To get the user name, we retrieve the password structure
  from the real user ID using the following function. */
  psswd_struct = getpwuid(userid);

  /* Print out the user ID with the name, in the same format as
  the id command. */
  printf("uid=%d(%s)\n", userid, psswd_struct->pw_name);
  return 0;
}