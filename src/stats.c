#include <stdio.h>
#include <stdlib.h>

#include "stats.h"

void print_usage()
{
  printf("htbench: wrong number of arguments\nUsage: htbench <hostname> <port_no> [options]\nOptions are:\n\t-n requests\tNumber of requests to perform\n\t-d duration duration of the test in seconds\n\t-t threads total number of threads to use");
  exit(2);
}