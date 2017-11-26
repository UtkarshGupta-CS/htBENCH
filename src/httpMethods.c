#include <stdio.h>  /* printf, sprintf */
#include <string.h> /* memcpy, memset, bzero, bcopy */
#include <unistd.h> /* read, write, close */
#include <stdlib.h> /* realloc, free */
#include <pthread.h> /* pthread_mutex_lock, pthread_mutex_unlock, pthread_mutex_destroy */

#include <sys/types.h>
#include <sys/socket.h> /* socket, connect, recv, shutdown, write */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */

#include "inputs.h"
#include "constants.h"
#include "utils.h" /* valIndex, error */
#include "httpMethods.h" /* getMethod */
#include "stats.h" /* print_stats */

/**
 * pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;
 * 
 * Mutex variables must be declared with type pthread_mutex_t, and must be initialized before they can be used.
  */
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

void getMethod(void *arguments)
{

  struct arg_struct *args = arguments;

  hostName = args->hostName;
  portNo = args->portNo;
  threadCount = args->threads;

  
  while (1)
  {    
    /**
     * pthread_mutex_lock (mymutex)
     * 
     * @params mymutex: mutex instance to be locked.
     * pthread_mutex_lock() routine is used by a thread to acquire a lock on the specified mutex variable.
       If the mutex is already locked by another thread, this call will block the calling thread until
       the mutex is unlocked. 
     */ 
    pthread_mutex_lock(&mutex1);

    if (completeReqCount >= args->noOfReq || currentTime() >= startTime + args->duration)
    {
      print_stats();
    }

    /**
     * pthread_mutex_unlock (mymutex)
     * 
     * @params mymutex: mutex instance to be unlocked.
     * pthread_mutex_unlock()will unlock a mutex if called by the owning thread.
       Calling this routine is required after a thread has completed its use of protected data if other
       threads are to acquire the mutex for their work with the protected data.
     * 
     */ 
    pthread_mutex_unlock(&mutex1);

    int sockfd, portno, n;
    /**
     * IPv4 socket address structure is named sockaddr_in and is defined by including the <netinet/in.h> header
     * struct sockaddr_in{  
          short sin_family;  Address family
          unsigned short sin_port;  IP port
          struct in_addr sin_addr;  IP address
          char sin_zero[8];  Padding to make structure the same size as SOCKADDR
        };  
      */
    struct sockaddr_in serv_addr;

    /**
     * 
     * struct hostent {
          char *h_name;        official name of host
          char **h_aliases;    alias list
          int  h_addrtype;     host address type
          int  h_length;       length of address
          char **h_addr_list;  list of addresses from name server
        };
      */
    struct hostent *server;
    char buffer[BUFFER_SIZE];

    portno = args->portNo;

    /**
     * socket -- create an endpoint for communication
     * int socket(int domain, int type, int protocol);
     *  
     * @params {{int}} domain -- specifies a communications domain within which communication will take place
     * @params {{int}} type -- which specifies the semantics of communication ex: sequenced, reliable, 
     two-way connection (SOCK_STREAM) based byte streams
     * @params {{int}} protocol -- specifies a particular protocol to be used with the socket.
     * @returns {{int}}
      */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
      error("htbench: ERROR, opening socket");

    /**
     * struct hostent *gethostbyname(const char *name);
     * 
     * @params {{char *}} address name
     * @returns {{int}} isreturn a pointer to an object with the  structure describing an internet host referenced by name
     */    
    server = gethostbyname(args->hostName);
    if (server == NULL)
    {
      error("htbench: ERROR, no such host\n");
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    
    /**
     * bcopy - copy byte sequence
     * 
     * void bcopy(const void *src, void *dest, size_t n);
     * 
     * @params {{const void *}} source
     * @params {{void *}} destination
     * @params {{size_t}} n bytes
      */
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);

    serv_addr.sin_port = htons(portno);

    /**
     * connect() function is used by a TCP client to establish a connection with a TCP server
     * 
     * int connect (int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);
     * 
     * @params {{int}} sockfd Specifies the file descriptor associated with the socket
     * @params {{const struct sockaddr *}} servaddr Points to a sockaddr structure containing the peer address
     * @parmas {{socklen_t}} addrlen Specifies the length of the sockaddr structure pointed to by the address argument
     * @returns {{int}} ex: returns 0 if the it succeeds in establishing a connection
      */
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
      failReqCount++;
      error("htbench: ERROR connecting");
    }
    
    n = write(sockfd, "GET / HTTP/1.0\r\n\r\n", strlen("GET / HTTP/1.0\r\n\r\n"));

    if (n < 0)
    {
      failReqCount++;
      error("htbench: ERROR writing to socket");
    }

    /**
     * bzero -- write zeroes to a byte string
     * 
     * void bzero(void *s, size_t n);
     * The bzero() function writes n zeroed bytes to the string s.  If n is zero, bzero() does nothing.
     * 
     * @params {{void *}} s input string
     * @params {{size_t}} n number of zeroed bytes
     */
    bzero(buffer, BUFFER_SIZE);

    char *tmpResult = NULL;

    // Below logic is to get the size of content received from response
    long thisReadSize;
    long totalBytesRead = 0;
    while (1)
    {
      // memset() is used to fill a block of memory with a particular value.
      memset(buffer, 0, BUFFER_SIZE);
      thisReadSize = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);

      if (thisReadSize <= 0)
        break;

      // realloc deallocates the old object pointed to by ptr and
      // returns a pointer to a new object that has the size specified by size
      tmpResult = (char *)realloc(tmpResult, thisReadSize + totalBytesRead);

      // memcpy() function copies n bytes from memory area src to memory area dest
      memcpy(tmpResult + totalBytesRead, buffer, thisReadSize);
      totalBytesRead += thisReadSize;
    }

    if (totalBytesRead <= 0)
    {
      failReqCount++;
      error("htbench: ERROR reading from socket");
    }
    else
    {
      pthread_mutex_lock(&mutex2);

      if (args->isVerbose == 1)
        printf("%s\n", tmpResult);

      contentLength = totalBytesRead;
      totalContentLength += totalBytesRead;

      // Retrieving the status code of the request
      char statusCodeForHttp10[4];
      strncpy(statusCodeForHttp10, tmpResult + valIndex(tmpResult, "HTTP/1.0"), 3);
      statusCodeForHttp10[3] = '\0';

      char statusCodeForHttp11[4];
      strncpy(statusCodeForHttp11, tmpResult + valIndex(tmpResult, "HTTP/1.1"), 3);
      statusCodeForHttp11[3] = '\0';

      if (checkStatusCode(statusCodeForHttp10) || checkStatusCode(statusCodeForHttp11))
      {
        completeReqCount++;
      }
      else
      {
        failReqCount++;
      }

      if (completeReqCount >= args->noOfReq || currentTime() >= startTime + args->duration)
      {
        print_stats();
      }

      free(tmpResult);
      pthread_mutex_unlock(&mutex2);
    }

    /**
     * shut down part of a full-duplex connection
     * int shutdown(int sockfd, int how);
     * "how" values as follows:
     * 0 {SHUT_RD}   -- Further receives are disallowed
     * 1 {SHUT_WR}   -- Further sends are disallowed
     * 2 {SHUT_RDWR} -- Further sends and receives are disallowed
      */
    shutdown(sockfd, SHUT_RDWR);
  }
  
  /**
   * pthread_mutex_destroy(mymutex)
   * 
   * @params mymutex: mutex instance to be destroyed.
   * pthread_mutex_destroy() should be used to free a mutex object which is no longer needed. 
    */
  
  pthread_mutex_destroy(&mutex1);
  pthread_mutex_destroy(&mutex2);
}
