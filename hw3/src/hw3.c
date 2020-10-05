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
#include <stdbool.h>
#include <math.h>

#define HOUGH_ROWS 100
#define HOUGH_COLS 100

uint8_t threshold_value = 110;

int main(int argc,char* argv[]) {
    if (argc==1) {
        analyzeImage();
    }
    if (argc>=2) {
        printf("please provide no inputs to the program.\n"); goto exit;
    }
exit:
    return 0;
}

void HTStraightLine(uint8_t **edge_map, int32_t n_rows, int32_t n_cols,
        uint16_t **HA, bool debug);

void analyzeImage()
{
    int32_t c, r, n_cols, n_rows;

    PNGFILE *pngfile = pngOpen(INPUT_PICTURE, "r");
    pngReadHdr(pngfile, &n_rows, &n_cols);

    uint8_t  **png_raw =    matalloc(n_rows, n_cols, 0, 0, sizeof(uint8_t));
    uint16_t **HA = matalloc(HOUGH_ROWS, HOUGH_COLS, 0, 0, sizeof(uint8_t));

    //store entire image into memory
    for (r = 0; r < n_rows; r++) pngReadRow(pngfile, png_raw[r]);

    HTStraightLine(png_raw, n_rows, n_cols, HA, true);
    printf("image is %d rows by %d cols\n", n_rows, n_cols);

    //scale HoughArray
    for (r = 0; r < HOUGH_ROWS; r++) {
        for (c  = 0; c < HOUGH_COLS; c++) {
            if (HA[r][c] > 255) HA[r][c] = 255;
        }
    }

    //TODO: NMS Non-Maxima Supression

    // write grayscale to file
    PNGFILE *output_png = pngOpen(OUTPUT_PICTURE, "w");
    pngWriteHdr(output_png, HOUGH_ROWS, HOUGH_COLS);
    for (r = 0; r < HOUGH_ROWS; r++) pngWriteRow(output_png, HA[r]);

    goto cleanup;
cleanup:
    matfree(png_raw);
    matfree(HA);
    pngClose(pngfile);
    pngClose(output_png);
}

/* @brief performs Hough Transform using rho/theta parameterization on an
 *      edge map and returns the accumulator
 * @param edge_map double pointer to psuedo-2D uint8_t array created by matalloc
 *      this argument is the edge map that the Hough Transform wil be performed
 *      on
 * @param n_rows uint8_t argument to define picture rows
 * @param n_cols uint8_t argument to define picture cols
 * @param HA double pointer to psuedo-2D array created by matalloc
 * @return void
 * @note writes values into HA
 */
void HTStraightLine(uint8_t **edge_map, int32_t n_rows, int32_t n_cols,
        uint16_t **HA, bool debug)
{
    uint8_t p=0;

    printf("rows: %d, cols: %d\n", n_rows, n_cols);

    // Step 1. Allocate a Hough Array H(p, o)
    // already done in main :)

    // Step 2. scan through edge map, E(x,y)
    /* for each edge point (x,y):
     *     For theta = (0,180):
     *         p = xcos(O)+ysin(O)
     *         H(O,p)++
     *     end
     * end
     */
    for (uint8_t r = 0; r < n_rows; r++) {
        for (uint8_t c = 0; c < n_cols; c++) {
            if (debug) printf("edge[%d][%d]: %d\n", r, c, edge_map[r][c]);
            for (int8_t theta = 0; theta <= 90; theta++) {
                p = r*cos(theta) + c*sin(theta);

                if (debug) printf(" theta: %d, p:%d\n", theta, p);
                if ( p >= 100) p = 99;

                //if (debug) printf("HA[p][theta]: %d\n", HA[p][theta]);
                HA[p][theta] += 1;
            }
        }
    }
}


