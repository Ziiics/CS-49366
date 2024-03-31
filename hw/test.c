/******************************************************************************
  Title : hwk3.c
  Author : Zivana C Syahputri
  Created on : March 21st, 2024
  Description : Able to print time interval for each suer
  Purpose : print last login
  Usage : lastlog [-a] [-s] [-f <filename>]
  Build with : gcc -o lo
  Modifications: March 25th, 2024
  Update the output
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <utmp.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <utmpx.h>

#define WTMPX_FILE "/var/log/wtmp"
#define UT_NAMESIZE 32

// struct user_check if the user was previously checked
struct user_check
{
  char user[UT_NAMESIZE];
  bool exist;
  struct user_check *next;
};
typedef struct user_check u_exist;

// struct user_login_check for the login and logout
struct user_login_check
{
  char user[UT_NAMESIZE];
  char device[UT_LINESIZE];
  time_t login_time;
  time_t logout_time;
  struct user_login_check *next;
};
typedef struct user_login_check u_login;

// struct user_time_check
struct user_time_check
{
  char user[UT_NAMESIZE];

  time_t range_time;
  struct user_time_check *next;
};
typedef struct user_time_check u_time;

u_time *user_time_head = NULL;

// add user data to user_check
void update_user_exist(u_exist **head, const char *user)
{
  u_exist *current = *head;
  while (current != NULL)
  {
    if (strncmp(current->user, user, UT_NAMESIZE) == 0)
    {
      return;
    }
  }

  u_exist *newNode = malloc(sizeof(u_exist));
  strcpy(newNode->user, user);
  newNode->exist = true;
  newNode->next = *head;
  *head = newNode;
  current = current->next;
}

// format time
void update_user_time(u_time **head, const char *user, time_t session_duration)
{
  u_time *current = *head;
  while (current != NULL)
  {
    if (strcmp(current->user, user) == 0)
    {
      current->range_time += session_duration; // Update existing time
      return;
    }
    current = current->next;
  }
  // User not found, add new
  u_time *newNode = malloc(sizeof(u_time));
  strcpy(newNode->user, user);
  newNode->range_time = session_duration;
  newNode->next = *head;
  *head = newNode;
}

// add user data to login_check (if user and !user)
void update_user_login(u_login **head, const char *user, const char *device, time_t time, int type)
{
  // for login process, add new user login time
  if (type == 1)
  {
    u_login *newNode = malloc(sizeof(u_login));
    strcpy(newNode->user, user);
    strcpy(newNode->device, device);
    newNode->login_time = time;
    newNode->logout_time = 0;
    newNode->next = *head;
    *head = newNode;
  }

  // for logout process, update logout time
  else
  {
    u_login *current = *head, *prev = NULL;
    while (current != NULL)
    {
      if (strncmp(current->device, device, UT_LINESIZE) == 0)
      {
        current->logout_time = time;
        time_t duration = current->logout_time - current->login_time;
        update_user_time(&user_time_head, device, duration);

        current->login_time = 0;
        current->logout_time = 0;
        return;
      }
      prev = current;
      current = current->next;
    }
    // printf("No matching user/session found for logout.\n");
  }
}

void print_session_times(u_time *head)
{
  printf("User Session Times:\n");
  u_time *current = head;
  while (current != NULL)
  {
    // Convert the total session time from seconds to hours, minutes, and seconds
    int hours = current->range_time / 3600;
    int minutes = (current->range_time % 3600) / 60;
    int seconds = current->range_time % 60;

    printf("%-12s: %d hours, %d minutes, %d seconds\n",
           current->device, hours, minutes, seconds);

    current = current->next;
  }
}

void free_user_time_list(u_time **head)
{
  u_time *current = *head;
  u_time *next = NULL;

  while (current != NULL)
  {
    next = current->next;
    free(current);
    current = next;
  }

  *head = NULL; // Ensure the head is NULL after freeing the list
}

// run thorugh wtmp file
int main(int argc, char *argv[])
{
  struct utmpx read_wtmp;
  int read_stat;
  char ch;
  bool showAllUser = false;
  bool showTotal = false;
  const char *filename = "/var/log/wtmp";

  while (true)
  {
    ch = getopt(argc, argv, "asf:");
    if (-1 == ch)
      break;

    // -a       Show the log times for all users that have entries in the file _PATH_WTMP.
    // -s       Produce a line of output at the end showing the totals for all users listed.
    // -f <file>  Use <file> instead of _PATH_WTMP.
    switch (ch)
    {
    case 'a':
      showAllUser = true;
      break;
    case 's':
      showTotal = true;
      break;
    case 'f':
      filename = optarg;
      break;
    case '?':
    case ':':
      printf("Option -%c requires an argument.\n", optopt);
      printf("Usage: %s [-a] [-s] [-f <filename>]\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  // read wtmp file or the user's file
  read_stat = open(filename, O_RDONLY);
  if (read_stat == -1)
  {
    perror("Could not open file data");
    abort();
  }

  u_exist *user_exist_head = NULL;
  u_login *user_login_head = NULL;
  while (read(read_stat, &read_wtmp, sizeof(read_wtmp)) == sizeof(read_wtmp))
  {

    // for user login information
    if (read_wtmp.ut_type == USER_PROCESS)
    {
      update_user_login(&user_login_head, read_wtmp.ut_user, read_wtmp.ut_line, read_wtmp.ut_tv.tv_sec, 1);
    }

    // for user logout information
    else if (read_wtmp.ut_type == DEAD_PROCESS)
    {
      update_user_login(&user_login_head, read_wtmp.ut_user, read_wtmp.ut_line, read_wtmp.ut_tv.tv_sec, 0);
    }
  }

  // print user_time_head when done processing everything
  print_session_times(user_time_head);
}


