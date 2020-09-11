/************************************************************************/
/* BLUR - PERFORM LOCAL-AVERAGE IMAGE FILTERING				*/
/*									*/
/* Usage:  blur [-n N] infile outfile					*/
/* Infile and outfile are grayscale PNG images.				*/
/* Perform N x N local averaging (default is 3 x 3).			*/
/*									*/
/* Ver. 2.0.5 (2019-10-3)						*/
/* (c) 2019 Jeffrey J. Rodriguez. All rights reserved.			*/
/* jjrodrig@email.arizona.edu						*/

#include "dip.h"

int main(int argc, char **argv)
{
  PNGFILE *fpx, *fpy;
  uint8_t **x, **y;
  int badswitch=0;
  int32_t N=3, NN, NN2, R, nrows, ncols, i, j, r, c;
  uint32_t sum;

  /* PARSE THE COMMAND LINE */
  progname = argv[0];
  while(--argc > 0 && **(argv+1) == '-')
    switch((*++argv)[1])
    {
    case 'n':
      /* If N > 4100 then sum can exceed UINT32_MAX, */
      /* and we would need float sum. */
      if(sscanf(*++argv, "%d", &N) != 1 || N < 3 || N%2 != 1
        || N > 4100)
        error("bad filter size");
      argc--;
      break;
    default:
      badswitch = 1;
    }
  R = (N-1)/2;
  if(badswitch || argc != 2)
  {
    fprintf(stderr, "Usage: %s [-n N] infile outfile\n", progname);
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
  /* The x array is extended by R pixels on all four sides. */
  /* The first element of the array is x[-R][-R]. */
  /* The first actual pixel of the image will be stored at x[0][0]. */
  x = (uint8_t **)matalloc(nrows+2*R, ncols+2*R,
    -R, -R, sizeof(uint8_t));
  y = (uint8_t **)matalloc(nrows, ncols, 0, 0, sizeof(uint8_t));
  if(x == NULL || y == NULL) error("can't allocate memory");

  /* READ THE IMAGE */
  for(r = 0; r < nrows; r++)
    pngReadRow(fpx, x[r]);

  /* PAD THE IMAGE BY REPLICATING PIXELS ACROSS THE BORDERS */
  padarray(x, nrows, ncols, R, sizeof(uint8_t));

  /* INIT */
  NN = N*N;
  NN2 = NN/2;

  /* COMPUTE THE BLURRED IMAGE */
  /* No bounds checking needed because we extended the image. */
  for(r = 0; r < nrows; r++)
    for(c = 0; c < ncols; c++)
    {
      sum = NN2; /* so that sum/NN will round to nearest int */
      for(i = -R; i <= R; i++)
        for(j = -R; j <= R; j++)
          sum += x[r+i][c+j];
      y[r][c] = sum/NN;
    }

  /* WRITE THE IMAGE */
  for(r = 0; r < nrows; r++)
    pngWriteRow(fpy, y[r]);

  /* CLOSE */
  pngClose(fpx);
  pngClose(fpy);
  exit(0);
}

