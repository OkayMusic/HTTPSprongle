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
void respond_to_boi(int boi, int *msg_type)
{
  if (*msg_type == 200)
  {
    FILE *open_html;
    const char *filename =
        "/home/richard/STORAGE/Documents/misc_programming/Sprongle/HTTPSprongle/index.html";
    open_html = fopen(filename, "r");

    if (open_html == NULL)
    {
      perror(filename);
      exit(1);
    }

    OK_headers(boi);
    OK_body(boi, open_html);

    fclose(open_html);
  }
}

void say_hello_to_boi(int boi, int *msg_type)
{
  // ideally the max http request size should be determined in andre_lukas.conf
  // 4096 is long enough for max internet explorer URL, so seems reasonable
  size_t buf_size = 4096;
  ssize_t msg_length;
  char buf[buf_size];
  msg_length = recv(bois[boi], buf, buf_size, 0);

  if (msg_length == -1)
    perror("Failed to receive hello message from boi");
  // recv returns 0 when the client calls their say_goodbye_to_boi
  else if (msg_length == 0)
    say_goodbye_to_boi(boi);
  else if (strncasecmp(buf, "GET", 3) == 0)
    *msg_type = 200;
}

void *
get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET)
  {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main()
{
  int sock_fd;
  // stores the type of message e.g. 200 (OK) 404 (NOT FOUND) etc.
  int msg_type;
  // The current client location in bois
  int boi = 0;
  // Set all clients to disconnected
  for (int b = 0; b < MAX_BOIS; ++b)
    bois[b] = -1;

  struct sockaddr_storage their_addr;
  socklen_t sin_size = sizeof their_addr;
  char s[INET6_ADDRSTRLEN];

  // initialize the socket's file descriptor, and listen on that socket
  the_feeling_of_rust_against_my_salad_fingers_is_almost_orgasmic(&sock_fd);

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

    if (fork() == 0)
    {
      say_hello_to_boi(boi, &msg_type);
      respond_to_boi(boi, &msg_type);
      say_goodbye_to_boi(boi);
    }

    // Increment the boi until there is a free slot (will hang
    // if all slots are taken)
    while (bois[boi] != -1)
      boi = (boi + 1) % MAX_BOIS;
  }

  return 0;
}
