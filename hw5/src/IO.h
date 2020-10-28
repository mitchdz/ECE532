#ifndef IO_H
#define IO_H

#include "../libdip/src/pngOpen.c"
#include "../libdip/src/matalloc.c"
#include <stdint.h>
#include "ECE576A.h"

/* @brief writePNG writes a png to a file
 * @param raw_data uint8_t pseudo 2-D array created by matalloc
 * @param n_rows number of rows of the pseudo 2D array
 * @param n_cols number of columns of the pseudo 2D array
 */
void writePNG(uint8_t** raw_data, char* filename, int n_rows, int n_cols)
{
    PNGFILE *output_png = pngOpen(filename, (char *)"w");
    pngWriteHdr(output_png, n_rows, n_cols);
    for (int r = 0; r < n_rows; r++) pngWriteRow(output_png, raw_data[r]);
    free(output_png);
}

error_ECE576A_t readPNGandClose(char *input_file, IMAGE img)
{
    PNGFILE *pngfile = pngOpen(input_file, (char *)"r");
    pngReadHdr(pngfile, &img.n_rows, &img.n_cols);

    // store entire image into memory
    for (int r = 0; r < img.n_rows; r++) pngReadRow(pngfile, img.raw_bits[r]);

    pngClose(pngfile);
    return E_ECE576A_SUCCESS;
}

#endif
