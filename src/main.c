#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "inputs.h"
#include "httpMethods.h"
#include "constants.h"

int main(int argc, char **argv)
{
  struct arg_struct args;
  args.argc = argc;
  args.argv = argv;
  pthread_t threads[NUM_THREADS];
  int rc;
  long t;
  for (t = 0; t < NUM_THREADS; t++)
  {
    printf("In main: creating thread %ld\n", t);
    rc = pthread_create(&threads[t], NULL, (void *)getMethod, &args);
    printf("%d", rc);
    if (rc)
    {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }

  pthread_exit(NULL);
}