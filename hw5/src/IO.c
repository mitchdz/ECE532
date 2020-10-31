#include "IO.h"

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

void readPNGandClose(char *input_file, IMAGE *img)
{   
    int nRows, nCols;
    PNGFILE *pngfile = pngOpen(input_file, (char *)"r");
    pngReadHdr(pngfile, &nRows, &nCols);

    img->n_rows = nRows;
    img->n_cols = nCols;

    uint8_t **raw_bits = matalloc(img->n_rows, img->n_cols, 0, 0, 
        sizeof(uint8_t));

    img->raw_bits = raw_bits;

    // store entire image into memory
    for (int r = 0; r < img->n_rows; r++) pngReadRow(pngfile, img->raw_bits[r]);

    pngClose(pngfile);
}