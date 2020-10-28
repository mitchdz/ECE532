/************************************************************************/
/* pngReadRow.c - READ A ROW OF A GRAYSCALE PNG IMAGE FILE		*/
/*									*/
/* prow = pointer to 1-D array where row will be stored			*/
/*									*/
/* Ver. 2.0.1 (2016-09-08)						*/
/* (c) 2016 Jeffrey J. Rodriguez. All rights reserved.			*/
/* jjrodrig@email.arizona.edu						*/

#include "pngio.h"

void pngReadRow(PNGFILE *fp, uint8_t *prow)
{
  png_read_row(fp->png_ptr, prow, NULL);
}
