/************************************************************************/
/* NEGATIVE - COMPUTE A NEGATIVE IMAGE					*/
/*									*/
/* Usage:  negative infile outfile					*/
/* Infile and outfile are grayscale PNG images.				*/
/*									*/
/* Ver. 2.0.2 (2016-09-11)						*/
/* (c) 2016 Jeffrey J. Rodriguez. All rights reserved.			*/
/* jjrodrig@email.arizona.edu						*/

#include "dip.h"

int main(int argc, char **argv)
{
  PNGFILE *fpx, *fpy;
  uint8_t *x, *y;
  int badswitch=0;
  int32_t nrows, ncols, r, c;

  /* PARSE THE COMMAND LINE */
  progname = argv[0];
  while(--argc > 0 && **(argv+1) == '-')
    switch((*++argv)[1])
    {
    default:
      badswitch = 1;
    }
  if(badswitch || argc != 2)
  {
    fprintf(stderr, "Usage: %s infile outfile\n", progname);
    exit(1);
  }

  /* OPEN FILES */
  if((fpx = pngOpen(*++argv, "r")) == NULL)
    error("can't open infile");
  if((fpy = pngOpen(*++argv, "w")) == NULL)
    error("can't open outfile");

  /* READ INFILE HEADER */
  pngReadHdr(fpx, &nrows, &ncols);
  
  /* WRITE OUTFILE HEADER */
  pngWriteHdr(fpy, nrows, ncols);

  /* ALLOCATE ARRAYS */
  x = (uint8_t *)malloc(ncols);
  y = (uint8_t *)malloc(ncols);
  if(x == NULL || y == NULL) error("can't allocate memory");

  /* COMPUTE THE NEGATIVE IMAGE & WRITE TO OUTFILE */
  for(r = 0; r < nrows; r++)
  {
    pngReadRow(fpx, x);
    for(c = 0; c < ncols; c++)
      y[c] = 255 - x[c];
    pngWriteRow(fpy, y);
  }

  /* CLOSE */
  pngClose(fpx);
  pngClose(fpy);
  exit(0);
}

