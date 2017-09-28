#include <stdio.h>  /* printf, sprintf */
#include <stdlib.h> /* exit */
#include <string.h> /* memcpy, memset */
#include <unistd.h> /* read, write, close */

#include <sys/types.h>
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */

#include <pthread.h>
#include "inputs.h"
#include "constants.h"
#include "utils.h"
#include "httpMethods.h"

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

void getMethod(void *arguments)
{

  struct arg_struct *args = arguments;

  while (args->counter != atoi(args->argv[5]))
  {
    pthread_mutex_lock(&mutex1);

    args->counter++;

    pthread_mutex_unlock(&mutex1);

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[BUFFER_SIZE];
    if (args->argc < 3)
    {
      fprintf(stderr, "usage %s hostname port\n", args->argv[0]);
      exit(0);
    }
    portno = atoi(args->argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
      error("ERROR opening socket");

    server = gethostbyname(args->argv[1]);
    if (server == NULL)
    {
      fprintf(stderr, "ERROR, no such host\n");
      exit(0);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
      error("ERROR connecting");

    n = write(sockfd, "GET /\r\n", strlen("GET /\r\n"));

    if (n < 0)
      error("ERROR writing to socket");

    bzero(buffer, BUFFER_SIZE);

    n = read(sockfd, buffer, BUFFER_SIZE - 1);

    if (n < 0)
      error("ERROR reading from socket");

    printf("%s\n", buffer);

    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
  }
}
