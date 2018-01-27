#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>

#include "globals.h"
#include "httpmessage.h"

/*******************************************************************************
This boy deals with serving the HTTP message. There's a good boy.
*******************************************************************************/

void OK_headers(int boi)
{
  char buf[1024];

  strcpy(buf, "HTTP/1.1 200 OK\r\n");
  if (send(bois[boi], buf, strlen(buf), 0) == -1)
    perror("Failed to send first header");
  strcpy(buf, SERVER);
  if (send(bois[boi], buf, strlen(buf), 0) == -1)
    perror("Failed to send second header");
  strcpy(buf, "Content-Type: text/html\r\n");
  if (send(bois[boi], buf, strlen(buf), 0) == -1)
    perror("Failed to send third header");
}

void NOTFOUND_headers(int boi)
{
  char buf[1024];
  strcpy(buf, "HTTP/1.1 404 NOT FOUND\r\n");
  if (send(bois[boi], buf, strlen(buf), 0) == -1)
    perror("Failed to send first header");
  strcpy(buf, SERVER);
  if (send(bois[boi], buf, strlen(buf), 0) == -1)
    perror("Failed to send second header");
  strcpy(buf, "Content-Type: text/html\r\n");
  if (send(bois[boi], buf, strlen(buf), 0) == -1)
    perror("Failed to send third header");
}

void HTML_body(int boi, FILE *open_html)
{
  size_t file_len;
  char *file_contents;
  char header_end[50];

  if (open_html == NULL)
    puts("kill yourself");

  // first work out the length of the file
  fseek(open_html, 0L, SEEK_END);
  file_len = ftell(open_html);
  rewind(open_html);

  // now populate file_contents
  if ((file_contents = (char *)malloc(file_len)) == NULL)
  {
    perror("Failed to allocate file_contents' memory, HTTP request failed");
  }
  for (int i = 0; i < file_len; ++i)
  {
    fread(file_contents, 1, file_len, open_html);
  }

  // okay, now we need to finish and send off the HTTP 1.1 header!
  if (sprintf(header_end, "%s%zd\n\r\n\r", "Content-Length: ", file_len) < 0)
    perror("Failed to prepare contents length header");
  if (send(bois[boi], header_end, strlen(header_end), 0) == -1)
    perror("Failed to send contents length header");

  // now we can send the fread in message
  if (send(bois[boi], file_contents, file_len, 0) == -1)
    perror("Failed to send http 1.1 body");

  free(file_contents);
}
