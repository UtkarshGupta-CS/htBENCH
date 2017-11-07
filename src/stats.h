#ifndef STATS_H

typedef struct resultStats
{
  int completeReqCount;
  int failReqCount;
  int timeTaken;
  int threadCount;
} resultStats;

void print_usage();

#endif // !STATS_H
