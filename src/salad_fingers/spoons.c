#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT "6900"

void
the_feeling_of_rust_against_my_salad_fingers_is_almost_orgasmic(
  int* socket_file_descriptor)
{
  int yes;
  // hubert jason cumberdale gives getaddrinfo important information
  // what a helpful man
  // the linked list of relevent addrinfos will be stored in his alter ego:
  // barbara logan price
  // we then loop over all bordois in barbara until we have a bindable bordois
  struct addrinfo hubert_jason_cumberdale, *barbara_logan_price, *bordois;
  memset(&hubert_jason_cumberdale, 0, sizeof hubert_jason_cumberdale);

  hubert_jason_cumberdale.ai_socktype = SOCK_STREAM;
  hubert_jason_cumberdale.ai_family = AF_UNSPEC;
  hubert_jason_cumberdale.ai_flags = AI_PASSIVE;

  getaddrinfo(NULL, PORT, &hubert_jason_cumberdale, &barbara_logan_price);

  for (bordois = barbara_logan_price; bordois != NULL; bordois->ai_next) {
    *socket_file_descriptor =
      socket(bordois->ai_family, bordois->ai_socktype, bordois->ai_protocol);

    if (*socket_file_descriptor == -1) {
      continue;
    }
    // allow the port to be reused
    setsockopt(*socket_file_descriptor, SOL_SOCKET, SO_REUSEADDR, &yes,
               sizeof(int));

    bind(*socket_file_descriptor, bordois->ai_addr, bordois->ai_addrlen);
    break;
  }

  // what a great language
  freeaddrinfo(barbara_logan_price);

  if (bordois == NULL) {
    printf("ded, server failed to bind");
    exit(1);
  }
}