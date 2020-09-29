/************************************************************************/
/* pngWriteRow.c - WRITE A ROW OF A GRAYSCALE PNG IMAGE FILE		*/
/*									*/
/* prow = pointer to 1-D array containing pixel data			*/
/*									*/
/* Ver. 2.0.1 (2016-09-08)						*/
/* (c) 2016 Jeffrey J. Rodriguez. All rights reserved.			*/
/* jjrodrig@email.arizona.edu						*/

#include "pngio.h"

void pngWriteRow(PNGFILE *fp, uint8_t *prow)
{
  png_write_row(fp->png_ptr, prow);
}
