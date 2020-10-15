/************************************************************************/
/* pngReadHdr.c - READ HEADER OF GRAYSCALE PNG IMAGE FILE		*/
/*									*/
/* The dimensions are returned as arguments.				*/
/* The function verifies that the image is a grayscale PNG image,	*/
/*									*/
/* Ver. 2.0.1 (2016-09-08)						*/
/* (c) 2016 Jeffrey J. Rodriguez. All rights reserved.			*/
/* jjrodrig@email.arizona.edu						*/

#include "pngio.h"

void pngReadHdr(PNGFILE *fp, int32_t *nrows, int32_t *ncols)
{
  char signature[8];
  png_uint_32 nrowsu, ncolsu;
  int bit_depth, color_type, interlace_type;
  int compression_type, filter_type, channels;

  if(fread(signature, 1, 8, fp->fileptr) != 8)
  {
    fprintf(stderr, "can't read PNG signature");
    exit(1);
  }
  if(png_sig_cmp((png_bytep)signature, (png_size_t)0, 8))
  {
    fprintf(stderr, "infile isn't a PNG image (wrong signature)");
    exit(1);
  }
  fp->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
    (png_voidp)NULL, pngErrorHandler, pngWarningHandler);
  fp->info_ptr = png_create_info_struct(fp->png_ptr);
  if(fp->info_ptr == NULL)
  {
    fprintf(stderr, "can't allocate info_ptr structure");
    exit(1);
  }
  png_init_io(fp->png_ptr, fp->fileptr);
  png_set_sig_bytes(fp->png_ptr, 8);
  png_read_info(fp->png_ptr, fp->info_ptr);
  png_get_IHDR(fp->png_ptr, fp->info_ptr, &ncolsu, &nrowsu, &bit_depth,
    &color_type, &interlace_type, &compression_type, &filter_type);
  if(nrowsu > UINT32_MAX || ncolsu > UINT32_MAX)
  {
    fprintf(stderr, "read_png_hdr: image dimensions exceed UINT32_MAX");
    exit(1);
  }
  *nrows = (int32_t)nrowsu; *ncols = (int32_t)ncolsu;
  channels = png_get_channels(fp->png_ptr, fp->info_ptr);
  if(channels != 1 || color_type != PNG_COLOR_TYPE_GRAY ||
    interlace_type != PNG_INTERLACE_NONE || bit_depth != 8)
    {
      fprintf(stderr, "read_png_hdr: not a normal grayscale image");
      exit(1);
    }
}
