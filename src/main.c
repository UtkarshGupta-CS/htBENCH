#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <unistd.h>

#include "inputs.h"
#include "httpMethods.h"
#include "constants.h"
#include "utils.h"
#include "stats.h"

int main(int argc, char **argv)
{

  if (argc < 2)
  {
    print_usage();
  }

  struct arg_struct args;
  int c, index;
  char *rvalue = NULL;
  char *tvalue = NULL;
  char *dvalue = NULL;
  char *hvalue = NULL;
  char *pvalue = NULL;

  opterr = 0;

  while ((c = getopt(argc, argv, "h:p:d:t:n:")) != -1)
  {
    switch (c)
    {
    case 'h':
      hvalue = optarg;
      printf("%s", optarg);
      args.hostName = optarg;
      break;

    case 'p':
      pvalue = optarg;
      printf("%s", optarg);
      args.portNo = atoi(optarg);
      break;
    case 'd':
      dvalue = optarg;
      printf("%s", optarg);
      args.duration = atoi(optarg);
      break;

    case 't':
      tvalue = optarg;
      printf("%s", optarg);
      args.threads = atoi(optarg);
      break;

    case 'n':
      rvalue = optarg;
      printf("%s", optarg);
      args.noOfReq = atoi(optarg);
      break;
    case '?':
      if (optopt == 'd' || optopt == 'n' || optopt == 't' || optopt == 'h' || optopt == 'p')
        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
      else if (isprint(optopt))
        fprintf(stderr, "Unknown option `-%c'.\n", optopt);
      else
        fprintf(stderr,
                "Unknown option character `\\x%x'.\n",
                optopt);
      return 1;
    default:
      abort();
    }
  }
  printf("dvalue = %s, tvalue = %s, rvalue = %s\n",
         dvalue, tvalue, rvalue);

  for (index = optind; index < argc; index++)
    printf("Non-option argument %s\n", argv[index]);

  args.argc = argc;
  args.argv = argv;

  args.completeReqCount = 0;

  int threadCount = args.threads;

  pthread_t threads[threadCount];

  int rc;
  long t;

  // stores the current time in the startTime global variable
  args.startTime = currentTime();

  for (t = 0; t < threadCount; t++)
  {
    printf("In main: creating thread %ld\n", t);

    rc = pthread_create(&threads[t], NULL, (void *)getMethod, &args);
    printf("%d", rc);
    if (rc != 0)
    {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(EXIT_FAILURE);
    }
  }
  for (t = 0; t < threadCount; t++)
  {
    pthread_join(threads[t], NULL);
  }

  return 0;
}