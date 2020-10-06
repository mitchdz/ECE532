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
#include <limits.h>
#include <stdbool.h>
#include <float.h>
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
        int32_t **HA);

void analyzeImage()
{
    int32_t c, r, n_cols, n_rows;

    PNGFILE *pngfile = pngOpen(INPUT_PICTURE, "r");
    pngReadHdr(pngfile, &n_rows, &n_cols);
    printf("image is %d rows by %d cols\n", n_rows, n_cols);

    uint8_t **png_raw = matalloc(n_rows, n_cols, 0, 0, sizeof(uint8_t));
    int32_t **HA      = matalloc(HOUGH_ROWS, HOUGH_COLS, 0, 0, sizeof(int32_t));
    uint8_t **HAout   = matalloc(HOUGH_ROWS, HOUGH_COLS, 0, 0, sizeof(uint8_t));

    //store entire image into memory
    for (r = 0; r < n_rows; r++) pngReadRow(pngfile, png_raw[r]);

    HTStraightLine(png_raw, n_rows, n_cols, HA);


    //scale HoughArray
    double max = DBL_MIN, min = DBL_MAX;
    for (r = 0; r < HOUGH_ROWS; r++) {
        for (c  = 0; c < HOUGH_COLS; c++) {
            max = (HA[r][c] > max) ? HA[r][c] : max;
            min = (HA[r][c] < min) ? HA[r][c] : min;
        }
    }

    for (r = 0; r < HOUGH_ROWS; r++) {
        for (c  = 0; c < HOUGH_COLS; c++) {
            HAout[r][c] = ((HA[r][c]-min)/(max-min))*255;
        }
    }


    //TODO: NMS (Non-Maxima Supression)

    // write grayscale to file
    PNGFILE *output_png = pngOpen(OUTPUT_PICTURE, "w");
    pngWriteHdr(output_png, HOUGH_ROWS, HOUGH_COLS);
    for (r = 0; r < HOUGH_ROWS; r++) pngWriteRow(output_png, HAout[r]);

    goto cleanup;
cleanup:
    matfree(png_raw);
    matfree(HA);
    pngClose(pngfile);
    pngClose(output_png);
    return;
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
        int32_t **HA)
{
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

    // we are doing 1) (b)
    // -pi/2 <= theta < pi
    // -n_rows <= p <= sqrt(r^2 + c^2)
    //
    int HA_rho_min = n_cols;
    int HA_rho_max = (int)sqrt(pow(n_rows,2)+pow(n_cols,2));

    printf("HA_rho_min: %d HA_rho_max: %d\n", HA_rho_min, HA_rho_max);

    int32_t r, c;
    int8_t HA_theta, HA_rho;
    double theta, p;
    bool valid_edge;
    #pragma omp parallel for private(r, c, p, theta) shared(HA) collapse(2)
    for (r = 0; r < n_rows; r++) {
        for (c = 0; c < n_cols; c++) {
            valid_edge = (edge_map[r][c] > 100) ? true : false;
            if (valid_edge) {
                for (theta = 0; theta < M_PI; theta += M_PI/99) {
                    p = r*cos(theta) + c*sin(theta);

                    // theta is between 0 and 99 in Hough Array
                    // theta value will be theta*99/M_PI
                    HA_theta = theta*99/M_PI;

                    // rho will be between 0 and 100 in the hough array,
                    // but the rho values can be between -N and Nsqrt(2)
                    // so in the case of the provided edge map we map
                    // -256 -> 352
                    // to
                    // 0 -> 99
                    // we calculate the scaled value as
                    // new value = (old_val - old_min)/(old_max - old_min) * new_max
                    HA_rho = ((p + HA_rho_min)/(HA_rho_max + HA_rho_min))*99;

                    //printf("r:%d\tc:%d\ttheta: %lf\tp:%lf\n", r, c, theta, p);
                    HA[HA_rho][HA_theta]++;
                }
            }
        }
    }
    return;
}

