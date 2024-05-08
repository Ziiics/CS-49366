/*****************************************************************************
  Description    : Clears the terminal window using ANSI escape
  Purpose        : To show how to use an ANSI escape sequence
  Usage          : clearscreen
  Build with     : gcc -o clearscreen -I../include clearscreen.c
*****************************************************************************/

/*

#define CLEAR_SCREEN "\033[2J" 
  // define macro named 'CLEAR_SCREEN', using the sequence '\033[2J' which is recognnized by terminal emulators
  // \033: ASCII escape character, represents the beginning of an escape sequence
     [: opening bracket, marking the start of a control sequence.
     2J: control sequence parameter that tells the terminal to clear the screen.
*/


#define _GNU_SOURCE
#include "common_hdrs.h"
#include <termios.h>  // provide temrinal control fucntions and data structures

#define CLEAR_SCREEN    "\033[2J" 

int main (int argc, char *argv[])
{
    printf(  "%s", CLEAR_SCREEN);
    return (0);
}
