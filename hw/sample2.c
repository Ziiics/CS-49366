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

void reset_terminal_mode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
    printf("\033[?1049l\033[2J\033[H"); // Switch back to the normal screen buffer and clear screen
}

void set_raw_mode() {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &orig_termios);
    new_termios = orig_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
    printf("\033[?1049h\033[2J\033[H"); // Switch to the alternate screen buffer and clear screen
    atexit(reset_terminal_mode);
}

void handle_sigint(int sig) {
    // Continue scrolling if paused
    paused = 0;
}

void handle_sigtstp(int sig) {
    // Pause scrolling
    paused = 1;
}

void handle_sigalrm(int sig) {
    if (!paused && display_start < total_lines - rows + 1) {
        display_start++;
    }
    alarm(scroll_interval);
}

void handle_sigterm(int sig) {
    reset_terminal_mode();
    printf("\nTerminated! Exiting...\n");
    exit(0);
}

void draw_status_bar() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[9];
    strftime(time_str, sizeof(time_str), "%H:%M:%S", t);
    printf("\033[%d;1H\033[7m%s   Lines: %d-%d\033[0m", rows, time_str, display_start + 1, display_start + rows - 1);
}

void display_file_content() {
    printf("\033[2J\033[H"); // Clear screen and move cursor to home position
    for (int i = 0; i < rows - 1; i++) {
        if (display_start + i < total_lines) {
            printf("\033[%d;1H%s", i + 1, lines[display_start + i]);
        }
    }
    draw_status_bar();
    printf("\033[%d;%dH", rows, cols - 2); // Move cursor to bottom right
    fflush(stdout);
}

void load_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    lines = malloc(MAX_LINES * sizeof(char *));
    if (!lines) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    total_lines = 0;

    while ((read = getline(&line, &len, file)) != -1) {
        if (total_lines < MAX_LINES) {
            lines[total_lines] = strdup(line);
            total_lines++;
        } else {
            break;
        }
    }

    free(line);
    fclose(file);
}

void handle_args(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "s:")) != -1) {
        switch (opt) {
            case 's':
                scroll_interval = atoi(optarg);
                if (scroll_interval < 1 || scroll_interval >= 60) {
                    fprintf(stderr, "Invalid scroll interval. Must be between 1 and 59 seconds.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                fprintf(stderr, "Usage: %s [-s secs] textfile\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Expected textfile argument after options\n");
        exit(EXIT_FAILURE);
    }

    load_file(argv[optind]);
}

int main(int argc, char *argv[]) {
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

    alarm(scroll_interval);

    while (1) {
        display_file_content();
        pause(); // Wait for signals
    }

    return EXIT_SUCCESS;
}
