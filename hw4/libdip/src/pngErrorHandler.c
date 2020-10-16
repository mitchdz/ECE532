/************************************************************************/
/* pngErrorHandler.c - PNG ERROR HANDLER				*/
/*									*/
/* This is called by pngReadHdr and pngWriteHdr.			*/
/*									*/
/* Ver. 2.0.1 (2016-09-08)						*/
/* (c) 2016 Jeffrey J. Rodriguez. All rights reserved.			*/
/* jjrodrig@email.arizona.edu						*/

#include "pngio.h"

void pngErrorHandler(png_structp png_ptr, png_const_charp msg)
{
  if(progname == NULL)
    fprintf(stderr, "libpng: %s\n", msg);
  else
    fprintf(stderr, "%s: libpng: %s\n", progname, msg);
  exit(1);
}
