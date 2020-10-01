/************************************************************************/
/* pngOpen.c - OPEN A GRAYSCALE PNG IMAGE FILE				*/
/*									*/
/* fname = file name							*/
/* mode = "r" or "w"							*/
/* The function returns a pointer to a PNGFILE structure if successful,	*/
/* else it returns NULL.						*/
/*									*/
/* Ver. 2.0.1 (2016-09-08)						*/
/* (c) 2016 Jeffrey J. Rodriguez. All rights reserved.			*/
/* jjrodrig@email.arizona.edu						*/

#include "pngio.h"

PNGFILE *pngOpen(char *fname, char *mode)
{
  PNGFILE *fp;

  if((fp = (PNGFILE *)malloc(sizeof(PNGFILE))) == NULL)
  {
    fprintf(stderr, "pngOpen: malloc failed");
    exit(1);
  }
  fp->mode = mode[0];

  switch(fp->mode)
  {
  case 'r':
    fp->fileptr = fopen(fname, "rb");
    break;
  case 'w':
    fp->fileptr = fopen(fname, "wb");
    break;
  default:
    fprintf(stderr, "pngOpen: invalid mode");
    exit(1);
  }
  return((fp->fileptr == NULL) ? NULL : fp);
}
