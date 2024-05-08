/*****************************************************************************
  Description    : Displays window size in lines and rows (pixels usually fails)
  Purpose        : To demonstrate the ioctl() call
  Usage          : winsize
  Build with     : gcc -Wall -g -o winsize winsize.c
*****************************************************************************/

/*

TIOSCGWINSZ - a macro used with 'ioctl' to get te window size of the terminal

struct winsize {
  unsigned short ws_row;	/* rows, in characters */
  unsigned short ws_col;	/* columns, in characters */
  unsigned short ws_xpixel;	/* horizontal size, pixels */
  unsigned short ws_ypixel;	/* vertical size, pixels */
};
  #include <sys/ioctl.h>
  // int ioctl(int fd, int cmd, ... );

int ioctl(int fd, unsigned long request, ...);
  // manipulates the underlying device parameters of special files
  int fd - file descriptor that represents an open file, socket, or other devices that supports device-specific I/O operations
  unsigned long request - device-dependent request code that determine the behavior of 'ioctl()'
  ... (varags) - optional arguments whose type and number depend on the 'request' code
  
*/


#include "common_hdrs.h"
#include <termios.h>


#ifndef TIOCGWINSZ
#include <sys/ioctl.h>
#endif

void get_winsize(int fd, unsigned short *rows, unsigned short *cols)
{
    struct winsize size;

    if (ioctl(fd, TIOCGWINSZ,  &size) < 0) {
        perror("TIOCGWINSZ error");
        return;
    }
    *rows = size.ws_row;
    *cols = size.ws_col;
}

/*****************************************************************************
                               Main Program
*****************************************************************************/

int main(int argc, char* argv[])
{
    unsigned short int rows, cols;

    if (isatty(STDIN_FILENO) == 0) {        // isatty: a function to check if a redirection is to a terminal
        fprintf(stderr, "Not a terminal\n");
        exit(1);
    }
    get_winsize(STDIN_FILENO, &rows, &cols);   /* print  size */
    if ( rows > 0 )
        printf("%d rows, %d columns\n", rows, cols);
    return 0;
}

