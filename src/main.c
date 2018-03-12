#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "globals.h"
#include "httpmessage.h"
#include "salad_fingers/spoons.h"

// Close the connection to the client bois[boi]
void say_goodbye_to_boi(int boi)
{
  if (bois[boi] == -1)
    return;
  if (close(bois[boi]) == -1)
    perror("Clingyboi");
}

// Respond to the client bois[boi]
void respond_to_boi(int boi, int *msg_type, char (*msg_contents)[4096])
{
  if (*msg_type == 200)
  {
    FILE *open_html;
    char filename[4096];
    strcpy(filename, path);
    strcat(filename, *msg_contents);
    puts(filename);

    open_html = fopen(filename, "r");

    // 404 NOT FOUND !!
    if (open_html == NULL)
    {
      open_html = fopen("NOTFOUND.html", "r");
      if (open_html == NULL)
        open_html = fopen("../ NOTFOUND.html", "r");

      if (open_html == NULL)
      {
        perror("Couldn't find NOTFOUND.html, pls leave this in HTTPSprongle/");
        exit(1);
      }

      NOTFOUND_headers(boi);
      HTML_body(boi, open_html);
    }
    // 200 OK FOUND!
    else
    {
      OK_headers(boi);
      HTML_body(boi, open_html);
    }
    fclose(open_html);
  }
}

void say_hello_to_boi(int boi, int *msg_type, char (*msg_contents)[4096])
{
  // ideally the max http request size should be determined in andre_lukas.conf
  // 4096 is long enough for max internet explorer URL, so seems reasonable
  size_t buf_size = 4096;
  ssize_t msg_length;
  char *temp; // we need this temp boy for strtok
  char buf[buf_size];

  msg_length = recv(bois[boi], buf, buf_size, 0);

  if (msg_length == -1)
    perror("Failed to receive hello message from boi");
  // recv returns 0 when the client calls their say_goodbye_to_boi
  else if (msg_length == 0)
    say_goodbye_to_boi(boi);
  else if (strncasecmp(buf, "GET", 3) == 0)
  {
    *msg_type = 200;
    strtok(buf, " ");
    temp = strtok(NULL, " ");

    // now copy temp over to msg_contents
    strcpy(*msg_contents, temp);

    // make sure requests end in a '/'
    if ((*msg_contents)[strlen(*msg_contents) - 1] != '/')
    {
      strcat(*msg_contents, "/");
    }

    // deal with requests which dont ask for index.html files
    if (strstr(*msg_contents, "index.html") == NULL)
    {
      strcat(*msg_contents, "index.html");
    }

    // haven't yet dealt with favicon requests, so just say goodbye on request
    if (strcasecmp(*msg_contents, "/favicon.ico") == 0)
    {
      say_goodbye_to_boi(boi);
      exit(0);
    }
  }
}

void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET)
  {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main()
{
  int sock_fd;             // serverside socket file descriptor
  int boi = 0;             // The current client location in bois
  int msg_type;            // stores the type of message e.g. 200 (OK) etc.
  char msg_contents[4096]; // buffer for important info relating to message

  // Set all clients to disconnected
  for (int b = 0; b < MAX_BOIS; ++b)
    bois[b] = -1;

  struct sockaddr_storage their_addr;
  socklen_t sin_size = sizeof their_addr;
  char s[INET6_ADDRSTRLEN];

  // initialize the socket's file descriptor, and listen on that socket
  the_feeling_of_rust_against_my_salad_fingers_is_almost_orgasmic(&sock_fd);
  init(); // parse through the config file, setting global variables

  if (listen(sock_fd, MAX_BOIS) == -1)
  {
    perror("Failed to listen on port 80");
    exit(-1);
  }

  printf("Server up! Waiting for connections...\n");

  while (1337)
  {
    // try to accept a new connection
    bois[boi] = accept(sock_fd, (struct sockaddr *)&their_addr, &sin_size);

    if (bois[boi] == -1)
      continue;

    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr),
              s, sizeof s);
    printf("Got connection to %s\n", s);

    //if (fork() == 0)
    {
      say_hello_to_boi(boi, &msg_type, &msg_contents);
      respond_to_boi(boi, &msg_type, &msg_contents);
      say_goodbye_to_boi(boi);
    }

    // Increment the boi until there is a free slot (will hang
    // if all slots are taken)
    while (bois[boi] != -1)
      boi = (boi + 1) % MAX_BOIS;
  }

  return 0;
}
