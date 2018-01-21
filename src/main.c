#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "salad_fingers/spoons.h"

#define MAX_BOIS 100 // Max clients

int bois[MAX_BOIS]; // Client file descriptors

// Close the connection to the client
// bois[boi]
void
say_goodbye_to_boi(int boi)
{
  if (bois[boi] == -1)
    return;
  close(bois[boi]);
}

// Respond to the client
// bois[boi]
void
respond_to_boi(int boi)
{
  send(bois[boi], "HTTP/1.1 200 OK\r\nServer: HTTPSprongle\r\nContent-Type: "
                  "text/html\r\nContent-Length: 20\r\n"
                  "\r\n"
                  "<big>Breetings</big>",
       200, 0);
  say_goodbye_to_boi(boi);
}

void*
get_in_addr(struct sockaddr* sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int
main()
{
  int sock_fd;
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
  listen(sock_fd, MAX_BOIS);

  printf("Server up! Waiting for connections...\n");

  while (1337) {
    // try to accept a new connection
    bois[boi] = accept(sock_fd, (struct sockaddr*)&their_addr, &sin_size);

    if (bois[boi] == -1)
      continue;

    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr*)&their_addr),
              s, sizeof s);
    printf("Got connection to %s\n", s);

    if (fork() == 0)
      respond_to_boi(boi);

    // Increment the boi until there is a free slot (will hang
    // if all slots are taken)
    while (bois[boi] != -1)
      boi = (boi + 1) % MAX_BOIS;
  }

  return 0;
}
