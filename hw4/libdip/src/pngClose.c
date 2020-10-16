/************************************************************************/
/* pngClose.c - CLOSE A PNG IMAGE FILE					*/
/*									*/
/* Ver. 2.0.1 (2016-09-08)						*/
/* (c) 2016 Jeffrey J. Rodriguez. All rights reserved.			*/
/* jjrodrig@email.arizona.edu						*/

#include "pngio.h"

void pngClose(PNGFILE *fp)
{
  if(fp->mode == 'w')
    png_write_end(fp->png_ptr, fp->info_ptr);
  if(fclose(fp->fileptr))
  {
    fprintf(stderr, "pngClose: fclose failed");
    exit(1);
  }
  if(fp->mode == 'r')
    png_destroy_read_struct(&fp->png_ptr, &fp->info_ptr, NULL);
  else
    png_destroy_write_struct(&fp->png_ptr, &fp->info_ptr);
  free(fp);
}
