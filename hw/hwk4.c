/*****************************************************************************
  Title          : hwk4.c
  Author         : Zivana C Syahputri
  Created on     : May 8th, 2024
  Description    : Displays and automatically scroll a file
  Purpose        : display a file, autonatically scrolling upward one line per second if needed
  Usage          : autoscroll [-s secs] textfile
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <time.h>
#include <sys/ioctl.h>

#define DEFAULT_SCROLL_INTERVAL 1
#define MAX_LINES 1000


struct termios orig_termios;
volatile sig_atomic_t paused = 0;
volatile sig_atomic_t scroll_interval = DEFAULT_SCROLL_INTERVAL;
char **lines = NULL;
int total_lines = 0;
int display_start = 0;
int rows, cols;

void reset_terminal_mode()
{
  tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
  printf("\033[2J\033[?1049l");
}

void set_raw_mode()
{
  struct termios new_termios;
  tcgetattr(STDIN_FILENO, &orig_termios);
  new_termios = orig_termios;
  new_termios.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
  printf("\033[?1049h\033[2J\033[1J\033[H");
}

void handle_sigint(int sig)
{
  paused = 0;
}

void handle_sigtstp(int sig)
{
  paused = 1;
}


void draw_status_bar(int actual_start_line, int actual_end_line)
{
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  char time_str[9];
  strftime(time_str, sizeof(time_str), "%H:%M:%S", t);
  printf("\033[%d;1H\033[7m%s   Lines: %d-%d\033[0m", rows, time_str, actual_start_line + 1, actual_end_line + 1);
}

void handle_sigalrm(int sig)
{
  static int seconds_counter = 0;
  draw_status_bar(display_start, display_start + rows - 2);
  fflush(stdout);
  seconds_counter++;

  if (seconds_counter >= scroll_interval)
  {
    if (!paused && display_start < total_lines - rows + 1)
    {
      display_start++;
    }
    seconds_counter = 0;
  }
  alarm(1);
}


void handle_sigterm(int sig)
{
  reset_terminal_mode();
  printf("\nTerminated! Exiting...\n");
  exit(0);
}



void display_file_content() {
    printf("\033[H\033[J"); 
    int current_line = 0;   // Index in the logical lines array
    int displayed_lines = 0;  // Current display line on the terminal
    int space_left = rows - 1;  // Space left for displaying lines (reserve one line for the status bar)

    while (display_start + current_line < total_lines && displayed_lines < space_left) {
        char *line = lines[display_start + current_line];
        int line_length = strlen(line);
        int lines_needed = (line_length + cols - 1) / cols;  // Calculate needed lines for wrapping

        if (displayed_lines + lines_needed <= space_left) {
            for (int j = 0; j < line_length; j += cols) {
                if (displayed_lines >= space_left)
                    break;
                printf("\033[%d;1H\033[2K%.*s", displayed_lines + 1, cols, &line[j]);
                displayed_lines++;  
            }
            current_line++;  
        } else {
            break;
        }
    }

    // Update the status bar
    draw_status_bar(display_start, display_start + displayed_lines - 1);
    printf("\033[%d;1H\033[2K%.*d", rows -1, cols, total_lines);
    printf("\033[%d;%dH", rows, cols - 2); // Move cursor to bottom right
    fflush(stdout);
}


void load_file(const char *filename)
{
  FILE *file = fopen(filename, "r");
  if (!file)
  {
    perror("fopen");
    exit(EXIT_FAILURE);
  }

  lines = malloc(MAX_LINES * sizeof(char *));
  if (!lines)
  {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  total_lines = 0;

  while ((read = getline(&line, &len, file)) != -1)
  {
    if (total_lines < MAX_LINES)
    {
      lines[total_lines] = strdup(line);
      total_lines++;
    }
    else
    {
      break;
    }
  }

  free(line);
  fclose(file);
}

void handle_args(int argc, char *argv[])
{ 
  if (argc % 2 != 0) {
    fprintf(stderr, "Usage: %s [-s secs] textfile\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  int opt;
  while ((opt = getopt(argc, argv, "s:")) != -1)
  {
    switch (opt)
    {
    case 's':
      scroll_interval = atoi(optarg);
      if (scroll_interval < 1 || scroll_interval >= 60)
      {
        fprintf(stderr, "Invalid scroll interval. Must be between 1 and 59 seconds.\n");
        exit(EXIT_FAILURE);
      }
      break;
    default:
      fprintf(stderr, "Usage: %s [-s secs] textfile\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  if (optind >= argc)
  {
    fprintf(stderr, "Expected textfile argument after options\n");
    exit(EXIT_FAILURE);
  }

  load_file(argv[optind]);
}

int main(int argc, char *argv[])
{
  handle_args(argc, argv);

  set_raw_mode();

  signal(SIGALRM, handle_sigalrm);
  signal(SIGINT, handle_sigint);
  signal(SIGTSTP, handle_sigtstp);
  signal(SIGTERM, handle_sigterm);
  signal(SIGQUIT, handle_sigterm);
  signal(SIGKILL, handle_sigterm);

  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  rows = w.ws_row;
  cols = w.ws_col;

  alarm(1);

  while (1)
  {
    display_file_content();
    pause(); 
  }

  return EXIT_SUCCESS;
}
