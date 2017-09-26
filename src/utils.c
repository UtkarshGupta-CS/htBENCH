#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

// Method that calls perror method and print out the error
void error(const char *msg)
{
  perror(msg);
  exit(0);
}