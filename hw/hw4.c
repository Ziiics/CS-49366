#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fnmatch.h>

#define MAX_DIRS 100

/* processing directory and comparing it*/
void process_dir(const char *path, char *pattern, char *slink)
{
  DIR *dp;
  struct dirent *entry;
  char full_path[PATH_MAX];
  char link_target[PATH_MAX];
  struct stat statbuf;
  char resolved_slink[PATH_MAX];

  if (slink && realpath(slink, resolved_slink) == NULL)
  {
    fprintf(stderr, "Error resolving path of %s\n", slink);
    exit(EXIT_FAILURE);
  }
  dp = opendir(path);
  if (dp == NULL)
  {
    fprintf(stderr, "Cannot open directory: %s", path);
    exit(EXIT_FAILURE);
  }

  while ((entry = readdir(dp)) != NULL)
  {
    snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;
    if (entry->d_type == DT_DIR)
      process_dir(full_path, pattern, slink);

    // find the file
    if (NULL != pattern)
      if (fnmatch(pattern, entry->d_name, 0) == 0)
        printf("%s\n", full_path);
        
    if (NULL != slink)
    {
      if (lstat(full_path, &statbuf) != 0)
      {
        fprintf(stderr, "Failed to get file: %s", full_path);
        exit(EXIT_FAILURE);
      }
      if (slink && S_ISLNK(statbuf.st_mode))
      {
        ssize_t len = readlink(full_path, link_target, sizeof(link_target) - 1);
        if (len != -1)
        {
          link_target[len] = '\0'; // Null-terminate the target
          char resolved_link[PATH_MAX];
          if (realpath(link_target, resolved_link) && strcmp(resolved_link, resolved_slink) == 0)
            printf("Found symlink to target: %s\n", full_path);
    
        }
        else
          fprintf(stderr, "Failed to read link: %s\n", full_path);
      }
    }
  }
  closedir(dp);
}

/* process user's argument */
int main(int argc, char *argv[])
{
  int ch;
  bool bool_slink = false;
  bool bool_pattern = false;
  char *slink = NULL;        // the file name to check the symbolic link of
  char *pattern = NULL;      // the string of the string pattern to search
  char *dir_names[MAX_DIRS]; // Array to store directory names
  int dir_count = 0;         // Count of directories added
  int index = 1;
  char *current_dir = "."; // will be checing current directory

  // if argc < 2, automatic exit
  if (argc < 2)
  {
    fprintf(stderr, "Usage: %s [dir1 dir2 ...] [-s <filename> | -m <fileglob>]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // get directories
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "-m") == 0)
    {
      break;
    }
    else if (dir_count < MAX_DIRS)
    {
      dir_names[dir_count++] = argv[i];
      index++;
      if (index + 2 > argc)
      {
        fprintf(stderr, "Usage: %s [dir1 dir2 ...] [-s <filename> | -m <fileglob>]\n", argv[0]);
        exit(EXIT_FAILURE);
      }
    }
    else
    {
      fprintf(stderr, "Maximum directory limit (100) exceeded\n");
      exit(EXIT_FAILURE);
    }
  }

  // get the rest of the argument
  optind = index;
  while ((ch = getopt(argc, argv, "s:m:")) != -1)
  {
    switch (ch)
    {
    case 's':
      bool_slink = true;
      slink = optarg;
      break;
    case 'm':
      bool_pattern = true;
      pattern = optarg;
      break;
    case '?':
    case ':':
      fprintf(stderr, "Usage: %s [dir1 dir2 ...] [-s <filename> | -m <fileglob>]\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  if ((bool_slink && slink[0] == '-') || (bool_pattern && pattern[0] == '-'))
  {
    fprintf(stderr, "Missing argument for -s or -m option\n");
    fprintf(stderr, "Usage: %s [dir1 dir2 ...] [-s <filename> | -m <fileglob>]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  if (dir_count == 0)
    process_dir(".", pattern, slink);
  else
    for (int i = 0; i < dir_count; i++)
      process_dir(dir_names[i], pattern, slink);
  return 0;
}


/*
Question:
1. Does it have to be sorted
2. Can both option be used together
3. It takes aboslute path and return relative path
*/