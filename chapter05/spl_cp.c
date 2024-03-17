#define _GNU_SOURCE
#include "../common_hdrs.h"
#include <libgen.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif
#define MESSAGE_SIZE 512
#define PERMISSIONS S_IRUSR   /* user read */| S_IWUSR   /* user read */| S_IRGRP   /* user read */| S_IWGRP   /* user read */| S_IROTH   /*  read */

int main(int argc, char *argv[])
{
  int source_fd;
  int target_fd;
  int num_bytes_read;
  int num_bytes_written;
  mode_t permissions = PERMISSIONS;
  char buffer[BUFFER_SIZE];
  char message[MESSAGE_SIZE];

  /* Check for correct usage.                                             */
  if (argc != 3)
  {
    sprintf(message, "%s source destination", basename(argv[0]));
    usage_error(message);
  }

  /* Open source file for reading.                                        */
  errno = 0;
  if ((source_fd = open(argv[1], O_RDONLY)) == -1)
  {
    sprintf(message, "unable to open %s for reading", argv[1]);
    fatal_error(errno, message);
  }
  /* Open target file for writing.                                        */
  errno = 0;
  if ((target_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC,
                        permissions)) == -1)
  {
    sprintf(message, "unable to open %s for writing", argv[2]);
    fatal_error(errno, message);
  }
  /* Start copying:
     Transfer BUFFER_SIZE bytes at a time from source_fd to target_fd.    */
  errno = 0;
  while ((num_bytes_read = read(source_fd, buffer, BUFFER_SIZE)) > 0)
  {
    errno = 0;
    num_bytes_written = write(target_fd, buffer, num_bytes_read);
    if (errno != 0)
      fatal_error(errno, "copy");
    else if (num_bytes_written != num_bytes_read)
    {
      sprintf(message, "write error to %s\n", argv[2]);
      fatal_error(-1, message);
    }
    errno = 0;
  }
  if (num_bytes_read == -1)
    fatal_error(errno, "error reading");

  /* Close files.                                                       */
  errno = 0;
  if (close(source_fd) == -1)
  {
    sprintf(message, "error closing source file %s", argv[1]);
    fatal_error(errno, message);
  }
  errno = 0;
  if (-1 == fsync(target_fd)) /* Flush data to device. */
    /* Error trying to flush data to device. */
    if (errno != EINVAL) /* If not a terminal */
      fatal_error(errno, "fsync");
  /* fsync() was successful. */
  errno = 0;
  if (close(target_fd) == -1)
  {
    sprintf(message, "error closing target file %s", argv[2]);
    fatal_error(errno, "error closing target file");
  }
  return 0;
}
