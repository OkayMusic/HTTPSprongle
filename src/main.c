#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
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

int main()
{
  int yeslolofc = 1;
  // file descriptor for our socket, fle descriptor for connection socket
  int sock_fd, new_fd;
  // Name A More Iconic Trio
  struct addrinfo hints, *server_info, *p;
  struct sockaddr_storage theiraddr;
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
  }

  return 0;
}
