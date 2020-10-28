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

#include "ECE576A.h"

typedef enum {
    UINT8_T,
    INT32_T,
} TYPE;

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
error_ECE576A_t zeroPsuedo2DArray(void** array, int32_t n_rows, int32_t n_cols,
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
    return E_ECE576A_SUCCESS;
}

/* @brief converts Pseudo 2D array to 1D array
 * @param grayscale pseudo 2D array created by matalloc
 * @param n_rows number of rows of the pseudo 2D array
 * @param n_cols number of columns of the pseudo 2D array
 * @param Histogram address that the 1D array should be stored to
 *
 * @see matalloc
 * @return error_t
 */
error_ECE576A_t convert2DPseudoArrayToHistogram(uint8_t **grayscale, int32_t n_rows,
        int32_t n_cols, uint8_t *Histogram);



error_ECE576A_t writeHistogramToFile(uint8_t *Histogram, const char *outfile);

error_ECE576A_t threshold2DPseudoArray(uint8_t** array, int32_t n_rows, int32_t n_cols,
    int32_t t)
{
    int32_t r, c;
    for (r = 0; r < n_rows; r++) {
        for (c = 0; c < n_cols; c++) {
            array[r][c] = (array[r][c] >= t) ? 255 : 0;
        }
    }
    return E_ECE576A_SUCCESS;
}

error_ECE576A_t findAdaptiveThreshold(IMAGE img);

#endif // THRESH_H
