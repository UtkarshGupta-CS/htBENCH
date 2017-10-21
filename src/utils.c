#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "utils.h"

// Method that calls perror method and print out the error
void error(const char *msg)
{
  perror(msg);
  exit(0);
}

// Method that returns current time in seconds
int currentTime()
{
  time_t curtime;

  /* 
    Note: the `tm` structure has the following definition −
    struct tm {
      int tm_sec;         // seconds,  range 0 to 59         
      int tm_min;         // minutes, range 0 to 59          
      int tm_hour;        // hours, range 0 to 23             
      int tm_mday;        // day of the month, range 1 to 31  
      int tm_mon;         // month, range 0 to 11             
      int tm_year;        // The number of years since 1900   
      int tm_wday;        // day of the week, range 0 to 6    
      int tm_yday;        // day in the year, range 0 to 365  
      int tm_isdst;       // daylight saving time             
    }; 
    */
  struct tm *loctime;

  /* Get the current time. */
  curtime = time(NULL);

  /* Convert it to local time representation. */
  loctime = localtime(&curtime);

  int seconds = loctime->tm_hour * 60 * 60 + loctime->tm_min * 60 + loctime->tm_sec;
  return seconds;
}