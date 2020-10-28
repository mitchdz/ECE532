/************************************************************************/
/* timediff.c - COMPUTE THE DIFFERENCE BETWEEN TWO TIMES		*/
/*									*/
/* The time difference is returned in units of seconds.			*/
/* Example of how to compute run time:					*/
/* #include <dip.h>							*/
/* struct timespec startTime, endTime;					*/
/* double runTime;							*/
/* if(clock_gettime(CLOCK_MONOTONIC, &startTime) != 0)			*/
/*   error("can't read clock");						*/
/* ...                                                                  */
/* if(clock_gettime(CLOCK_MONOTONIC, &endTime) != 0)			*/
/*   error("can't read clock");						*/
/* runTime = timediff(startTime, endTime);				*/
/*									*/
/* Ver. 2.0.1 (2016-09-08)						*/
/* (c) 2016 Jeffrey J. Rodriguez. All rights reserved.			*/
/* jjrodrig@email.arizona.edu						*/

#include <time.h>

#define BILLION	1000000000L

double timediff(struct timespec t1, struct timespec t2)
{
  struct timespec tdiff;

  if(t2.tv_nsec < t1.tv_nsec)
  {
    tdiff.tv_sec = (t2.tv_sec-1)-t1.tv_sec;
    tdiff.tv_nsec = BILLION-(t1.tv_nsec-t2.tv_nsec);
  }
  else
  {
    tdiff.tv_sec = t2.tv_sec-t1.tv_sec;
    tdiff.tv_nsec = t2.tv_nsec-t1.tv_nsec;
  }
  return( (double)tdiff.tv_sec + (double)tdiff.tv_nsec/BILLION );
}
