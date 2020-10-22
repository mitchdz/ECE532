/************************************************************************/
/* PNGIO.H - HEADER FILE FOR PNG I/O FUNCTIONS              */
/*                                  */
/* Ver. 2.0.0 (2016-09-08)                      */
/* (c) 2016 Jeffrey J. Rodriguez. All rights reserved.          */
/* jjrodrig@email.arizona.edu                       */

#ifndef _pngio_h
#define _pngio_h

/* INCLUDES */
#include <stdio.h>  /* for fprintf(), etc.     */
#include <stdlib.h> /* for malloc(), exit()    */
#include <string.h> /* for strcmp()        */
#include <stdint.h> /* for uint8_t, etc.       */
#include <png.h>    /* for libpng declarations */

/* TYPEDEFS */
typedef struct {
  FILE *fileptr;
  png_structp png_ptr;
  png_infop info_ptr;
  char mode;}
  PNGFILE;

/* DECLARATIONS OF GLOBAL VARIABLES */
extern const char *progname; /* for error handler */

/* FUNCTION DECLARATIONS */
extern PNGFILE *pngOpen(char *fname, char *mode);
extern void pngReadHdr(PNGFILE *fp, int *nrows, int *ncols);
extern void pngWriteHdr(PNGFILE *fp, int nrows, int ncols);
extern void pngReadRow(PNGFILE *fp, uint8_t *prow);
extern void pngWriteRow(PNGFILE *fp, uint8_t *prow);
extern void pngClose(PNGFILE *fp);
extern void pngErrorHandler(png_structp png_ptr, png_const_charp msg);
extern void pngWarningHandler(png_structp png_ptr, png_const_charp msg);

#endif
