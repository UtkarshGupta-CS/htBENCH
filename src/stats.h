
#ifndef STATS_H

typedef struct resultStats
{
  int completeReqCount;
  int failReqCount;
  int timeTaken;
  int threadCount;
  char *contentLength;
} resultStats;

int checkStatusCode(char *statusCode);

void print_usage();

#endif // !STATS_H
