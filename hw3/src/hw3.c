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

    //store entire image into memory
    for (r = 0; r < n_rows; r++) pngReadRow(pngfile, png_raw[r]);

    //zero out Hough Array
    for (r = 0; r < HOUGH_ROWS; r++)
        for (c = 0; c < HOUGH_COLS; c++)
            HA[r][c] = 0;

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

    // write Hough Array to file
    writePNG(HAout, OUTPUT_HOUGH_ARRAY, HOUGH_ROWS, HOUGH_COLS);

    // NMS to detect Hough Peaks
    for (r = 0; r < HOUGH_ROWS; r++)
        for (c  = 0; c < HOUGH_COLS; c++)
            HAout[r][c] = (HAout[r][c] < threshold_value) ? 0 : HAout[r][c];

    // write detected Hough Peaks to file
    writePNG(HAout, OUTPUT_HOUGH_PEAKS, HOUGH_ROWS, HOUGH_COLS);

    // organize hough peaks into list
    HoughPeakNode* head = (HoughPeakNode*)malloc(sizeof(HoughPeakNode));
    populateHoughPeaksList(HAout, head, n_rows, n_cols);

    printf("There are %d Hough Peaks with a threshold of %d\n",
            getNumberHoughPeakNodes(head), threshold_value);

    // print all hough peaks
    printf("\n");
    printf("Hough Peaks:\n");
    printHoughPeaks(head);
    printf("\n");
    printf("Hough lines as equations:\n");
    printHoughLineEquations(head);
    printf("\n");

    printf("Writing Hough Array to %s\n", OUTPUT_HOUGH_ARRAY);
    printf("Writing Hough Peaks to %s\n", OUTPUT_HOUGH_PEAKS);

    goto cleanup;
cleanup:
    freeHoughPeakNodeList(head);
    matfree(png_raw);
    matfree(HA);
    matfree(HAout);
    pngClose(pngfile);
    return;
}

/* @brief populateHoughPeaksList takes a Hough Array
 * which is a pseudo 2-D uint8_t array created by matalloc
 * which is returned from HTStraightLine
 *
 * @param HA pseudo 2-D uint8_t Hough Array
 * @param head head to HoughPeakNode list
 */
void populateHoughPeaksList(uint8_t** HA, HoughPeakNode* head,
        int32_t n_rows,
        int32_t n_cols)
{
    // We can compute the line in the image space by using
    // y =           m           * x +      b
    // y = -cos(theta)/sin(theta)* x + p/sin(theta)
    double m, b, rho, theta;
    for (int r = 0; r < HOUGH_ROWS; r++) {
        for (int c  = 0; c < HOUGH_COLS; c++) {
            if (HA[r][c] >= threshold_value) {

                theta = -M_PI/2 + c*M_PI/HOUGH_COLS;

                /* so rho should be the two lines below this, but for some
                * reason I get more sensicle rho values when I simply
                * let rho equal the rho index. This is silly.
                */
                //int HA_rho_min = -(int)round(sqrt(pow(n_rows,2)+pow(n_cols,2)));
                //rho = (2*r)/HOUGH_COLS + HA_rho_min;
                rho = r;

                // m will just equal -infinity if theta=0
                // and we don't care about the slope in that case anyways
                if (theta == 0) b = rho;
                else            b = rho/sin(theta);

                // m = slope
                m = (-cos(theta)/sin(theta));

                HoughPeakNode* node = \
                                (HoughPeakNode*)malloc(sizeof(HoughPeakNode));
                node->intensity = HA[r][c];
                node->theta = theta;
                node->rho = rho;
                node->m = m;
                node->b = b;
                node->next = NULL;

                appendHoughPeakNode(head, node);
            }
        }
    }
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

    /* we are doing 1) (b)
     * -pi/2 <= theta < pi/2
     * -Nmax <= p <= Nmax
     * where Nmax to sqrt(n_rows^2 + n_cols^2)
     */

    //int HA_rho_min = -((n_cols+n_rows)/2);
    //int HA_rho_min = 0;
    int HA_rho_max = (int)round(sqrt(pow(n_rows,2)+pow(n_cols,2)));
    int HA_rho_min = -HA_rho_max;

    int32_t r, c;
    int8_t HA_rho;
    double theta, p;
    bool valid_edge;
    //pragma omp parallelizes the OUTER 2 loops w/ collapse(2) argument
    //pragma omp not used anymore, but left here for fun
    //#pragma omp parallel for private(r, c, p, theta) shared(HA) collapse(2)
    for (r = n_rows-1; r >0; r--) {
      for (c = 0; c < n_cols; c++) {
        //valid_edge checks if the pixel is an edge or not
        valid_edge = (edge_map[r][c] > 0) ? true : false;
        if (valid_edge) {
          int theta_counter=0;
          for (theta = -M_PI/2; theta < M_PI/2; theta += M_PI/HOUGH_COLS) {
            //classic rho algorithm
            p = c*cos(theta) + r*sin(theta);

            // rho will be between 0 and 99 in the hough array,
            // but the rho values can be between -N and Nsqrt(2)
            // so in the case of the provided edge map we map
            // -256 -> 352
            // to
            // 0 -> 99
            // we calculate the scaled value as
            // new value = (old_val - old_min)/(old_max - old_min) * new_max
            HA_rho = \
              round(((p - HA_rho_min)/(HA_rho_max - HA_rho_min))*(HOUGH_COLS-1));

            //printf("r:%d\tc:%d\ttheta: %lf\tp:%lf\n", r, c, theta, p);
            HA[HA_rho][theta_counter]++;
            theta_counter++;
          }
        }
      }
    }
    return;
}
