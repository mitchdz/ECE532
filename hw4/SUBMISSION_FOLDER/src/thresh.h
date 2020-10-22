#ifndef THRESH_H
#define THRESH_H

#define _USE_MATH_DEFINES
#include <math.h>

#include <stdint.h>
#include <stdio.h>
// include all of the libdip header and source files
// note: this looks awful. Instead we could just include
// the .h and .c files and provide path to gcc with -I../libdip/src
// but alas I am here not doing that.
#include "../libdip/src/pngio.h"
#include "../libdip/src/matfree.c"
#include "../libdip/src/pngReadRow.c"
#include "../libdip/src/padarray.c"
#include "../libdip/src/pngErrorHandler.c"
#include "../libdip/src/pngReadHdr.c"

#include "IO.h"

typedef enum {
    UINT8_T,
    INT32_T,
} TYPE;

/* hw4 error codes for debugging the program */
enum _hw4_error
{
    E_SUCCESS,
    E_FILE_NOT_FOUND,
    E_GENERIC_ERROR,
    E_ARRAY_SIZE_MISMATCH,
    E_NOT_IMPLEMENTED,
    E_RECURSIVE_FIRST_LAST_NOT_FOUND,
};

typedef enum _hw4_error error_t;

/* helper function to easily decode error message */
struct _errordesc{
    int code;
    char *message;
} errordesc[] = {
    { E_SUCCESS,  (char *)"No error" },
    { E_FILE_NOT_FOUND, (char *)"File not found" },
    { E_GENERIC_ERROR,  (char *)"Generic Error" },
    { E_ARRAY_SIZE_MISMATCH,  (char *)"Array sizes do not match" },
    { E_NOT_IMPLEMENTED,  (char *)"Not implemented yet" },
    { E_RECURSIVE_FIRST_LAST_NOT_FOUND, (char *)"Recursive first and last index not able to be found"},
};

void printError(error_t E, char *msg)
{
    printf("Error %d: %s %s\n", errordesc[E].code, errordesc[E].message ,msg);
}

/* @brief zeros a psuedo 2D array created by matalloc
 *
 * @param array pseudo 2D array
 * @param n_rows number of rows of the array
 * @param n_cols number of columns of the array
 * @param type what type the array is using the enum TYPE
 *
 * @see matalloc
 * @return error_t
 */
error_t zeroPsuedo2DArray(void** array, int32_t n_rows, int32_t n_cols,
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

/* @brief Recursive Update Formula, this is the formula that determines
 * each value in the histogram based on a threshold
 *
 * @param h histogram as a pointer to a uint8_t array
 * @param t int32_t threshold value
 *
 * @return error_t
 */
error_t RecursiveUpdateFormula(uint8_t *h, int *threshold, double *Hvalues);



error_t thresholdPngWithKittler(char *input_file);
error_t convert2DPseudoArrayToHistogram(uint8_t **grayscale, int32_t n_rows,
        int32_t n_cols, uint8_t *Histogram);
#endif // THRESH_H


