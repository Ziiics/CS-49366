#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <utmp.h>
#include <errno.h>
#include <time.h>

#define WTMPX_FILE "/var/log/wtmp"

struct 

struct session_time
{
  long login_time;
  long logout_time; // If 0, session might be open
  struct session_time *next;
};

struct user_session
{
  char user[32]; // Adjust based on UT_NAMESIZE
  struct session_time *sessions;
  struct user_session *next;
};

struct user_session *sessions = NULL;

struct user_session *find_or_create_user(const char *user)
{
  struct user_session *current = sessions;
  while (current)
  {
    if (strcmp(current->user, user) == 0)
    {
      return current;
    }
    current = current->next;
  }
  struct user_session *new_user = malloc(sizeof(struct user_session));
  strncpy(new_user->user, user, sizeof(new_user->user) - 1);
  new_user->user[sizeof(new_user->user) - 1] = '\0';
  new_user->sessions = NULL;
  new_user->next = sessions;
  sessions = new_user;
  return new_user;
}

void add_session_time(struct user_session *user, long time, int is_login)
{
  if (is_login)
  {
    struct session_time *new_session = malloc(sizeof(struct session_time));
    new_session->login_time = time;
    new_session->logout_time = 0; // Open session
    new_session->next = user->sessions;
    user->sessions = new_session;
  }
  else
  { // Handle as logout
    if (user->sessions && user->sessions->logout_time == 0)
    { // Update the most recent session if logout is missing
      user->sessions->logout_time = time;
    }
  }
}

void close_open_sessions(long time)
{
  struct user_session *user = sessions;
  while (user)
  {
    if (user->sessions && user->sessions->logout_time == 0)
    { // Close open session
      user->sessions->logout_time = time;
    }
    user = user->next;
  }
}

void process_wtmp()
{
  int fd = open(WTMPX_FILE, O_RDONLY);
  if (fd == -1)
  {
    perror("Failed to open wtmp file");
    exit(EXIT_FAILURE);
  }

  struct utmp ut;
  ssize_t bytesRead;

  while ((bytesRead = read(fd, &ut, sizeof(ut))) > 0)
  {
    if (ut.ut_type == USER_PROCESS)
    {
      printf("This is a login: %-12s %d\n", ut.ut_name, ut.ut_tv.tv_sec);
      struct user_session *user = find_or_create_user(ut.ut_user);
      add_session_time(user, ut.ut_tv.tv_sec, 1);
    }
    else if (ut.ut_type == DEAD_PROCESS)
    {
      printf("This is a lgOut: %-12s %d\n", ut.ut_name, ut.ut_tv.tv_sec);
      struct user_session *user = find_or_create_user(ut.ut_user);
      add_session_time(user, ut.ut_tv.tv_sec, 0);
    }
    else
    {
      printf("Unknown yet atm: %-12s %d\n", ut.ut_name, ut.ut_type);
      close_open_sessions(ut.ut_tv.tv_sec);
    }
  }

  if (bytesRead == -1)
  {
    perror("Error reading wtmp file");
  }

  close(fd);
}

void print_sessions()
{
  struct user_session *user = sessions;
  while (user)
  {
    printf("User: %s\n", user->user);
    struct session_time *session = user->sessions;
    while (session)
    {
      if (session->login_time > 0)
      {
        printf("\tSession: Login at %ld", session->login_time);
        if (session->logout_time > 0)
        {
          printf(", Logout at %ld, Duration: %ld seconds\n",
                 session->logout_time, session->logout_time - session->login_time);
        }
        else
        {
          printf(" (Logout missing)\n");
        }
      }
      session = session->next;
    }
    user = user->next;
  }
}

int main()
{
  process_wtmp();
  print_sessions();
  return 0;
}
