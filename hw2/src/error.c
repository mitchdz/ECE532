/************************************************************************/
/* error.c - PRINT ERROR MESSAGE					*/
/*									*/
/* This prints an error message and exits the program with status = 1.	*/
/*									*/
/* Ver. 2.0.1 (2016-09-08)						*/
/* (c) 2016 Jeffrey J. Rodriguez. All rights reserved.			*/
/* jjrodrig@email.arizona.edu						*/

#include "dip.h"

const char *progname=NULL;

void error(const char *msg)
{
  if(progname == NULL)
    fprintf(stderr, "%s\n", msg);
  else
    fprintf(stderr, "%s: %s\n", progname, msg);
  exit(1);
}

