/************************************************************************/
/* padarray.c - PAD AN IMAGE ACROSS ITS BORDERS BY REPLICATING PIXELS	*/
/*									*/
/* The input matrix "x" is a pseudo-2D array of type uint8_t or		*/
/* float or double, and the array must have been allocated using the	*/
/* matalloc() function, so that negative indexing is allowed.		*/
/* To pad an array by some						*/
/* amount across each of its borders, the array must have been		*/
/* allocated to include extra rows and columns beyond the main image	*/
/* region. Assume the main image region begins at (0,0). Then the	*/
/* allocated array must include extra rows and columns beyond the	*/
/* main region (0:nrows-1, 0:ncols-1), as follows:			*/
/*   x = matalloc(nrows+2*amount, ncols+2*amount, -amount, -amount,	*/
/*   dataTypeSize)							*/
/* The "amount" parameter is the number of rows or columns to be	*/
/* added across each of the four borders.				*/
/* After allocating the array, you must load the image into the middle	*/
/* portion of the array, i.e. at coordinates (0:nrows-1, 0:ncols-1),	*/
/* thereby leaving empty elements along the borders outside the image.	*/
/* The padarray() function will then fill in those empty elements	*/
/* with replicated image pixel values. 					*/
/* For example, if amount = 2, then x[0][-1] and x[0][-2] will be	*/
/* assigned the value of x[0][0].					*/
/*									*/
/* Ver. 2.0.5 (2019-10-03)						*/
/* (c) 2019 Jeffrey J. Rodriguez. All rights reserved.			*/
/* jjrodrig@email.arizona.edu						*/

#include "dip.h"

void padarray(void *x, int nrows, int ncols, int amount, int size)
{
  int i,j;
  uint8_t **c;
  float **f;
  double **d;

  if(x == NULL) error("padarray: array ptr is NULL");
  if(amount < 0 || amount > nrows || amount > ncols)
    error("padarray: bad amount to pad");

  switch(size)
  {
  case sizeof(uint8_t):
    c = x;
    for(i=0;i<nrows;i++)
      for(j=1;j<=amount;j++)
      {
        c[i][-j]=c[i][0];
        c[i][ncols+j-1]=c[i][ncols-1];
      }
  
    for(i=0;i<amount;i++)
      for(j=-amount;j<ncols+amount;j++)
      {
        c[-i-1][j]=c[0][j];
        c[nrows+i][j]=c[nrows-1][j];
      }
    break;
  case sizeof(float):
    f = x;
    for(i=0;i<nrows;i++)
      for(j=1;j<=amount;j++)
      {
        f[i][-j]=f[i][0];
        f[i][ncols+j-1]=f[i][ncols-1];
      }
  
    for(i=0;i<amount;i++)
      for(j=-amount;j<ncols+amount;j++)
      {
        f[-i-1][j]=f[0][j];
        f[nrows+i][j]=f[nrows-1][j];
      }
    break;
  case sizeof(double):
    d = x;
    for(i=0;i<nrows;i++)
      for(j=1;j<=amount;j++)
      {
        d[i][-j]=d[i][0];
        d[i][ncols+j-1]=d[i][ncols-1];
      }
  
    for(i=0;i<amount;i++)
      for(j=-amount;j<ncols+amount;j++)
      {
        d[-i-1][j]=d[0][j];
        d[nrows+i][j]=d[nrows-1][j];
      }
    break;
  default:
    error("padarray: invalid size of data type");
  }
}
