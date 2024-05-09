/*****************************************************************************
  Description    : Demonstrates use of ANSI escape sequences
  Purpose        : To show the effects of various ANSI escape sequences
  Usage          : vt100ctrl
  Build with     : gcc -o escapes escapes.c
  Modifications  : January 23, 2012
                   Comments revised and a few extra sequences added.

  Notes          :
  This program lets the user see the effects of various escape sequences and
  how they can be used. The choice of escapes shown here is pretty random.

  Every ANSI escape sequence begins with the character whose code is
  decimal code 27, or octal 033, which we will write as ESC. Many codes
  begin with a two-character sequence consisting of ESC followed by '[',
  although there are also escape sequences that use just the ESC.
*****************************************************************************/

#define _GNU_SOURCE
#include "common_hdrs.h"
#include <termios.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

/******************************************************************************/
/*                      Selected VT100 Escape Sequences                       */
/******************************************************************************/

#define CLEAR_LINE      "\033[1A\033[2K\033[G"
#define CLEAR_SCREEN    "\033[2J"
#define CLEAR_ABOVE     "\033[1J"
#define CLEAR_DOWN      "\033[0J"
#define INVERSE_START    "\033[7m"
#define INVERSE_END     "\033[27m"
#define PROMPT            "prompt: "
#define CURSOR_HOME     "\033[1;1H"
#define CURSOR_PARK     "\033[24;80H"
#define CURSOR_UP_2     "\033[2A"
#define SCROLL_WINDOW   "\033M"


/* Move the cursor to the screen position (line,col) (upper left is (0,0)   */
void moveto(int line, int col )
{
    char seq_str[20];

    sprintf(seq_str, "\033[%d;%dH", line, col);
    write(STDOUT_FILENO, seq_str, strlen(seq_str));
}

/****************************************************************************/
/*                           Main Program                                     */
/****************************************************************************/

int main (int argc, char *argv[])
{
    int    i;
    char*  line; 

    // This is how to test whether output has been redirected:
    if ( !isatty(fileno(stdout)) )
        fatal_error(errno, "isatty");

    line = calloc(512, sizeof(char));
    if ( NULL == line  )
        exit(EXIT_FAILURE);

    // Clear the screen.
    // setting up terminal screen
    sprintf(line,  "%s%s", CLEAR_SCREEN,CURSOR_HOME);
    write(1,line, strlen(line));

    // positioning the cursor and printing a character
    sprintf( line, "\033[1;10H +");
    write(1,line, strlen(line));

    // draw plus signs at increasing positions
    for ( i = 4; i< 20; i+= 4) {
        sprintf(line, "\033[%d;%dH+", i, i);
        write(1,line, strlen(line));
    }

    // furthur cursor movement and printing
    sprintf(line, "\033[30E +");
    write(1,line, strlen(line));

    // memory clean up
    free(line);
    return (0);
}
