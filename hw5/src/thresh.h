#ifndef THRESH_H
#define THRESH_H

#define _USE_MATH_DEFINES
#include <math.h>
#include "ECE576A.h"

#include <stdint.h>
#include <stdio.h>
// include all of the libdip header and source files
// note: this looks awful. Instead we could just include
// the .h and .c files and provide path to gcc with -I../libdip/src
// but alas I am here not doing that.
#include "../libdip/src/pngio.h"
#include "../libdip/src/dip.h"

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
 */
void zeroPsuedo2DArray(void** array, int32_t n_rows, int32_t n_cols,
        TYPE type);

/* @brief converts Pseudo 2D array to 1D array
 * @param grayscale pseudo 2D array created by matalloc
 * @param n_rows number of rows of the pseudo 2D array
 * @param n_cols number of columns of the pseudo 2D array
 * @param Histogram address that the 1D array should be stored to
 *
 * @see matalloc
 * @return error_t
 */
void convert2DPseudoArrayToHistogram(uint8_t **grayscale, int32_t n_rows,
        int32_t n_cols, uint8_t *Histogram);



void writeHistogramToFile(uint8_t *Histogram, const char *outfile);

void threshold2DPseudoArray(uint8_t** array, int32_t n_rows, int32_t n_cols,
    int32_t t);

#endif // THRESH_H
