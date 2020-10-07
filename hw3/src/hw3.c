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

uint8_t threshold_value = 225;

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

void writePNG(uint8_t** raw_data, char* filename, int n_rows, int n_cols)
{
    PNGFILE *output_png = pngOpen(filename, "w");
    pngWriteHdr(output_png, n_rows, n_cols);
    for (int r = 0; r < n_rows; r++) pngWriteRow(output_png, raw_data[r]);
    free(output_png);
}

void analyzeImage()
{
    int32_t c, r, n_cols, n_rows;

    //initialize input picture
    PNGFILE *pngfile = pngOpen(INPUT_PICTURE, "r");
    pngReadHdr(pngfile, &n_rows, &n_cols);
    printf("image is %d rows by %d cols\n", n_rows, n_cols);

    //initial pseudo 2D arrays
    uint8_t **png_raw = matalloc(n_rows, n_cols, 0, 0, sizeof(uint8_t));
    int32_t **HA      = matalloc(HOUGH_ROWS, HOUGH_COLS, 0, 0, sizeof(int32_t));
    uint8_t **HAout   = matalloc(HOUGH_ROWS, HOUGH_COLS, 0, 0, sizeof(uint8_t));
    uint8_t **HAlines = matalloc(n_rows, n_cols, 0, 0, sizeof(uint8_t));

    //store entire image into memory
    for (r = 0; r < n_rows; r++) pngReadRow(pngfile, png_raw[r]);

    // call HTStraightLine function
    HTStraightLine(png_raw, n_rows, n_cols, HA);

    //find max and min value in Hough Array
    double max = DBL_MIN, min = DBL_MAX;
    for (r = 0; r < HOUGH_ROWS; r++) {
        for (c  = 0; c < HOUGH_COLS; c++) {
            max = (HA[r][c] > max) ? HA[r][c] : max;
            min = (HA[r][c] < min) ? HA[r][c] : min;
        }
    }

    //properly scale hough array between 0 and 255
    for (r = 0; r < HOUGH_ROWS; r++)
        for (c  = 0; c < HOUGH_COLS; c++)
            HAout[r][c] = ((HA[r][c]-min)/(max-min))*255;

    // write grayscale to file
    writePNG(HAout, OUTPUT_HOUGH_ARRAY, HOUGH_ROWS, HOUGH_COLS);

    // NMS to detect Hough Peaks
    for (r = 0; r < HOUGH_ROWS; r++)
        for (c  = 0; c < HOUGH_COLS; c++)
            HAout[r][c] = (HAout[r][c] < threshold_value) ? 0 : HAout[r][c];

    // write detected Hough Peaks to file
    writePNG(HAout, OUTPUT_HOUGH_PEAKS, HOUGH_ROWS, HOUGH_COLS);

    // Convert Hough Peaks in rho/theta to lines in cartesian
    for (r = 0; r < n_rows; r++)
        for (c  = 0; c < n_cols; c++)
            HAlines[r][c] = 0;

    // create head of singly linked list
    HoughPeakNode* head = (HoughPeakNode*)malloc(sizeof(HoughPeakNode));
    // We can compute the line in the image space by using
    // y =           m           * x +      b
    // y = -cos(theta)/sin(theta)* x + p/sin(theta)
    double m, b, rho, theta;
    for (r = 0; r < HOUGH_ROWS; r++) {
        for (c  = 0; c < HOUGH_COLS; c++) {
            if (HAout[r][c] >= threshold_value) {
                rho = r;
                theta = c;

                m = (-cos(theta)/sin(theta));
                b = rho/sin(theta);
                HoughPeakNode* node = \
                                (HoughPeakNode*)malloc(sizeof(HoughPeakNode));
                node->theta = theta;
                node->rho = rho;
                node->m = m;
                node->b = b;
                node->next = NULL;

                appendHoughPeakNode(head, node);
            }
        }
    }

    //TODO: write lines on HALines

    printf("There are %d Hough Peaks with a threshold of %d\n",
            getNumberHoughPeakNodes(head), threshold_value);

    printf("Writing Hough Array to %s\n", OUTPUT_HOUGH_ARRAY);
    printf("Writing Hough Peaks to %s\n", OUTPUT_HOUGH_PEAKS);
    printf("TODO: Writing Hough Lines to %s\n", OUTPUT_HOUGH_LINES);

    printf("TODO: superimpose hough lines with %s\n", INPUT_PICTURE);

    goto cleanup;
cleanup:
    freeHoughPeakNodeList(head);
    matfree(png_raw);
    matfree(HA);
    matfree(HAout);
    matfree(HAlines);
    pngClose(pngfile);
    return;
}





/* @brief performs Hough Transform using rho/theta parameterization on an
 *      edge map and returns the accumulator
 * @param edge_map double pointer to psuedo-2D uint8_t array created by matalloc
 *      this argument is the edge map that the Hough Transform wil be performed
 *      on
 * @param n_rows uint8_t argument to define picture rows
 * @param n_cols uint8_t argument to define picture cols
 * @param HA double pointer to psuedo-2D int32_t array created by matalloc
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
    // 0 <= theta < pi
    // -n_rows <= p <= sqrt(r^2 + c^2)

    //setting N to n_cols seems good enough.
    //future could be (n_rows+n_cols)/2
    int HA_rho_min = n_cols;
    int HA_rho_max = (int)sqrt(pow(n_rows,2)+pow(n_cols,2));

    int32_t r, c;
    int8_t HA_theta, HA_rho;
    double theta, p;
    bool valid_edge;
    //pragma omp parallelizes the OUTER 2 loops w/ collapse(2) argument
    #pragma omp parallel for private(r, c, p, theta) shared(HA) collapse(2)
    for (r = 0; r < n_rows; r++) {
        for (c = 0; c < n_cols; c++) {
            //valid_edge checks if the pixel is an edge or not
            valid_edge = (edge_map[r][c] > 100) ? true : false;
            if (valid_edge) {
             for (theta = 0; theta < M_PI; theta += M_PI/99) {
              //classic rho algorithm
              p = r*cos(theta) + c*sin(theta);

              // theta is between 0 and 99 in Hough Array
              // theta value will be theta*99/M_PI
              HA_theta = round(theta*99/M_PI);

              // rho will be between 0 and 100 in the hough array,
              // but the rho values can be between -N and Nsqrt(2)
              // so in the case of the provided edge map we map
              // -256 -> 352
              // to
              // 0 -> 99
              // we calculate the scaled value as
              // new value = (old_val - old_min)/(old_max - old_min) * new_max
              HA_rho = round(((p + HA_rho_min)/(HA_rho_max + HA_rho_min))*99);

              //printf("r:%d\tc:%d\ttheta: %lf\tp:%lf\n", r, c, theta, p);
              HA[HA_rho][HA_theta]++;
             }
            }
        }
    }
    return;
}
