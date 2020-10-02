#include <stdio.h>
#include <stdlib.h>
#include "pngio.h"
#include "pngOpen.c"
#include "pngReadHdr.c"
#include "pngErrorHandler.c"
#include "hw3.h"
#include "matfree.c"
#include "pngReadRow.c"
#include "padarray.c"
#include <math.h>

int main(int argc,char* argv[])
{
    if (argc==1) {
        analyzeImage();
    }
    if (argc>=2) {
        printf("please provide no inputs to the program.\n"); goto exit;
    }
exit:
    return 0;
}

void analyzeImage()
{
    int32_t c, r, n_cols, n_rows;

    PNGFILE *pngfile = pngOpen(INPUT_PICTURE, "r");
    pngReadHdr(pngfile, &n_rows, &n_cols);
    printf("image is %d rows by %d cols\n", n_rows, n_cols);

    uint8_t **png_raw =       matalloc(n_rows, n_cols, 0, 0, sizeof(uint8_t));
    double **HTMagnitudes =   matalloc(n_rows, n_cols, 0, 0, sizeof(double));
    uint8_t **HTGrayScale =   matalloc(n_rows, n_cols, 0, 0, sizeof(double));

    //store entire image into memory
    for (r = 0; r < n_rows; r++) pngReadRow(pngfile, png_raw[r]);

    // store JJR and SG magnitudes of each pixel
    for (r = 1; r < n_rows - 1; r++) {
        for (c = 1; c < n_cols - 1; c++) {
            //TODO: HT function for each pixel
            //TODO: HTGrayScale
        }
    }

    // write grayscale to file
    PNGFILE *output_png = pngOpen(OUTPUT_PICTURE, "w");
    pngWriteHdr(output_png, n_rows, n_cols);
    for (r = 0; r < n_rows; r++) pngWriteRow(output_png, HTGrayScale[r]);

    goto cleanup;
cleanup:
    matfree(png_raw);
    matfree(HTMagnitudes);
    matfree(HTGrayScale);
    pngClose(pngfile);
    pngClose(output_png);
}
