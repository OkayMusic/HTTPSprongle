#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"

int bois[MAX_BOIS]; // Client file descriptors
char path[4096];    // Store the path to the folder we will serve from

void init(void)
{
  FILE *config;
  size_t bufsize = 4096;
  char *buf;
  char *lhs;
  char *rhs;

  // Here I assume that the program is run from the HTTPSprongle directory OR
  // the build directory directly. Solving the general problem of finding the
  // location of the executable at runtime is OS dependent and lame.
  config = fopen("andre_lukas.conf", "r");
  if (config == NULL)
  {
    config = fopen("../andre_lukas.conf", "r");
  }

  if (config == NULL)
  {
    perror("Fatal: couldn't find config file! Try running program from build "
           "directory");
    exit(-1);
  }

  while (getline(&buf, &bufsize, config) > 0)
  {
    lhs = strtok(buf, ": ");
    rhs = strtok(NULL, " \r\n");

    // for every new item in the config file we just need another equivalent if
    // statement
    if (strcasecmp(lhs, "serve_from") == 0)
    {
      strcpy(path, rhs); // safe, as path must always be longer than rhs
    }
  }
}