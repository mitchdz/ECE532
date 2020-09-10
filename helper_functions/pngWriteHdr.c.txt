/************************************************************************/
/* pngWriteHdr.c - WRITE HEADER OF GRAYSCALE PNG IMAGE FILE		*/
/*									*/
/* The dimensions nrows x ncols are passed as parameters.		*/
/* The png_ptr and info_ptr structures are created.			*/
/* The header is written to the image file.				*/
/*									*/
/* Ver. 2.0.1 (2016-09-08)						*/
/* (c) 2016 Jeffrey J. Rodriguez. All rights reserved.			*/
/* jjrodrig@email.arizona.edu						*/

#include "pngio.h"

void pngWriteHdr(PNGFILE *fp, int32_t nrows, int32_t ncols)
{
  int bit_depth=8, color_type=PNG_COLOR_TYPE_GRAY;
  int interlace_type=PNG_INTERLACE_NONE;
  int compression_type=PNG_COMPRESSION_TYPE_BASE;
  int filter_type=PNG_FILTER_TYPE_BASE;

  fp->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
    (png_voidp)NULL, pngErrorHandler, pngWarningHandler);
  fp->info_ptr = png_create_info_struct(fp->png_ptr);
  if(fp->info_ptr == NULL)
  {
    fprintf(stderr, "can't allocate info_ptr structure");
    exit(1);
  }
  png_init_io(fp->png_ptr, fp->fileptr);
  png_set_IHDR(fp->png_ptr, fp->info_ptr, ncols, nrows, bit_depth,
    color_type, interlace_type, compression_type, filter_type);
  png_write_info(fp->png_ptr, fp->info_ptr);
}
