#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "inputs.h"
#include "httpMethods.h"
#include "constants.h"
#include "utils.h"
#include "stats.h"

int main(int argc, char **argv)
{
  struct arg_struct args;
  args.argc = argc;
  args.argv = argv;

  args.completeReqCount = 0;

  int threadCount = atoi(argv[4]);

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