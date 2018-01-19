#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netdb.h>

// FIRST test to see if we can establish any simple tcp connection
// using port 6900 for now

/*
WHAT IS THE POINT:
call getaddrinfo()

get the socket file descriptor via socket()

allow the socket to be used by more than 1 client using setsockopt()

call bind()

*/

#define PORT "6900"
#define BACKLOG 10

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
  int yeslolofc = 1;
  // file descriptor for our socket, fle descriptor for connection socket
  int sock_fd, new_fd;
  // Name A More Iconic Trio
  struct addrinfo hints, *server_info, *p;
  struct sockaddr_storage their_addr;

  struct sigaction sa;

  socklen_t sin_size;

  char s[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints);

  // TCP connection
  hints.ai_socktype = SOCK_STREAM;
  // IPV4/6
  hints.ai_family = AF_UNSPEC;
  // use my ip
  hints.ai_flags = AI_PASSIVE;

  getaddrinfo(NULL, PORT, &hints, &server_info);

  for (p = server_info; p != NULL; p->ai_next)
  {
    // attempt to set up a socket with the data in this addrinfo
    if ((sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    {
      continue;
    }

    // allow the port to be reused
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yeslolofc, sizeof(int));

    // call bind
    bind(sock_fd, p->ai_addr, p->ai_addrlen);
    break;
  }

  // nice language
  freeaddrinfo(server_info);

  if (p == NULL)
  {
    printf("server failed to bind\n");
  }

  listen(sock_fd, BACKLOG);

  printf("Server up! Waiting for connections...\n");

  while (1337)
  {
    sin_size = sizeof their_addr;

    // try to accept a new connection
    new_fd = accept(sock_fd, (struct sockaddr *)&their_addr, &sin_size);

    if (new_fd == -1)
    {
      continue;
    }

    inet_ntop(their_addr.ss_family,
              get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);

    printf("Got connection to %s\n", s);

    if (!fork())
    {
      close(sock_fd);
      send(new_fd, "Breetings", 9, 0);
      close(new_fd);
      exit(0);
    }
    close(new_fd);
  }

  return 0;
}
