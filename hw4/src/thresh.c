#include <stdio.h>
#include <stdlib.h>
#include "pngio.h"
#include "pngOpen.c"
#include "pngReadHdr.c"
#include "pngErrorHandler.c"
#include "thresh.h"
#include "matfree.c"
#include "pngReadRow.c"
#include "padarray.c"
#include <limits.h>
#include <stdbool.h>
#include <float.h>
#include <math.h>

#define HOUGH_ROWS 100
#define HOUGH_COLS 100

int8_t threshold_value = -1; // user defined or Kittler's method

char *INPUT_FILE = "";
char *OUTPUT_FILE = "";


void printHelp()
{
    //TODO: print help menu
}

/*
 * call the program as
 * thresh [-t T] infile outfile
 *
 * T: optional user specified threshold 0 <= T <= 255;
 *  default: T is automatically computed using Kittler's method
 *
 * infile: the input grayscale image
 *
 * outfile: the output bi-level image
 */
int main(int argc,char* argv[]) {
    if (argc==1) { //just calling the program by itself
        printf("Error: provide correct amount of inputs");
        printHelp();
    }
    if (argc==2) { // calling program with a single param
        printf("Error: provide correct amount of inputs");
        printHelp();
    }
    if (argc==3) { // calling program with 2 params
        INPUT_FILE = argv[2]; //TODO: error check
        OUTPUT_FILE = argv[3]; //TODO: error check
        analyzeImage();
    }
    if (argc==4) { //not enough params
        printf("Error: provide correct amount of inputs");
        printHelp();
    }
    if (argc==5) { // defining threshold
        threshold_value = atoi(argv[2]); //TODO: error check
        INPUT_FILE = argv[3]; //TODO: error check
        OUTPUT_FILE = argv[4]; //TODO: error check
        analyzeImage();
    }
    if (argc>5) { //too many params
        printf("Error: provide correct amount of inputs");
        printHelp();
    }
exit:
    return 0;
}

typedef enum {
    UINT8_T,
    INT32_T,
} TYPE;


_hw4_error zeroPsuedo2DArray(void** array, int32_t n_rows, int32_t n_cols, \
        TYPE type)
{
    int32_t r, c;
    //zero out Hough Array
    for (r = 0; r < n_rows; r++) {
        for (c = 0; c < n_cols; c++) {
            switch(type) {
                case INT32_T:
                    ((int32_t**)array)[r][c] = 0;
                    break;
                case UINT8_T:
                    ((uint8_t **)array)[r][c] = 0;
                    break;
            }
        }
    }
    return E_SUCCESS;
}


void analyzeImage()
{
    int32_t c, r, n_cols, n_rows;
    //initialize input picture
    PNGFILE *pngfile = pngOpen(INPUT_FILE, "r");
    pngReadHdr(pngfile, &n_rows, &n_cols);
    printf("image is %d rows by %d cols\n", n_rows, n_cols);

    //initial pseudo 2D arrays
    uint8_t **png_raw = matalloc(n_rows, n_cols, 0, 0, sizeof(uint8_t));
    int32_t **HA      = matalloc(HOUGH_ROWS, HOUGH_COLS, 0, 0, sizeof(int32_t));
    uint8_t **HAout   = matalloc(HOUGH_ROWS, HOUGH_COLS, 0, 0, sizeof(uint8_t));

    //store entire image into memory
    for (r = 0; r < n_rows; r++) pngReadRow(pngfile, png_raw[r]);

    zeroPsuedo2DArray(HA, HOUGH_ROWS, HOUGH_COLS, INT32_T);


    goto cleanup;
cleanup:
    matfree(png_raw);
    matfree(HA);
    matfree(HAout);
    pngClose(pngfile);
}
