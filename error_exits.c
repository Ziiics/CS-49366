#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void error_mssge(int errornum, const char *msg)
{
  if (errornum > 0) /* an errno value */
    fprintf(stderr, "%s\n", strerror(errornum));
  else /* project-defined error number - ignore and just print msg */
    fprintf(stderr, "%s\n", msg);
}
void fatal_error(int errornum, const char *msg)
{
  error_mssge(errornum, msg);
  exit(EXIT_FAILURE);
}
void usage_error(const char *msg)
{
  fprintf(stderr, "usage: %s\n", msg);
  exit(EXIT_FAILURE);
}