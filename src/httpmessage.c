#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>

#include "globals.h"
#include "httpmessage.h"

/*******************************************************************************
This boy deals with serving the HTTP message. There's a good boy.
*******************************************************************************/

void
headers(int boi)
{
  char buf[1024];

  strcpy(buf, "HTTP/1.1 200 OK\r\n");
  send(bois[boi], buf, strlen(buf), 0);
  strcpy(buf, SERVER);
  send(bois[boi], buf, strlen(buf), 0);
  strcpy(buf, "Content-Type: text/html\r\n");
  send(bois[boi], buf, strlen(buf), 0);
  strcpy(buf, "\r\n");
  send(bois[boi], buf, strlen(buf), 0);
}

void
body(int boi, FILE* open_html)
{
  // send messages 1024 bytes at a time (this seems like a reasonable # to use)
  char buf[CHUNK];
  size_t chars_read;

  while (chars_read = fread(&buf, 1, sizeof(buf), open_html)) {
    send(bois[boi], buf, chars_read, 0);
  }
}
