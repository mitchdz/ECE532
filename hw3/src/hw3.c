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
#define HOUGH_COLS 91

//uint8_t threshold_value = 225;
uint8_t threshold_value = 210;

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

    // organize hough peaks into list
    HoughPeakNode* head = (HoughPeakNode*)malloc(sizeof(HoughPeakNode));
    populateHoughPeaksList(HAout, head);

    // zero out HAlines pseudo 2D array
    for (r = 0; r < n_rows; r++)
        for (c  = 0; c < n_cols; c++)
            HAlines[r][c] = 0;

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
    //writeHoughLinesFromNode(HAlines, n_rows, n_cols, head);

    // write Hough Lines to file
    //writePNG(HAlines, OUTPUT_HOUGH_LINES, n_rows, n_cols);

    //TODO: superimpose houghlines and input img

    printf("Writing Hough Array to %s\n", OUTPUT_HOUGH_ARRAY);
    printf("Writing Hough Peaks to %s\n", OUTPUT_HOUGH_PEAKS);
    //printf("TODO: Writing Hough Lines to %s\n", OUTPUT_HOUGH_LINES);

    //printf("TODO: superimpose hough lines with %s\n", INPUT_PICTURE);

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

/* @brief populateHoughPeaksList takes a Hough Array
 * which is a pseudo 2-D uint8_t array created by matalloc
 * which is returned from HTStraightLine
 *
 * @param HA pseudo 2-D uint8_t Hough Array
 * @param head head to HoughPeakNode list
 */

void populateHoughPeaksList(uint8_t** HA, HoughPeakNode* head)
{
    // We can compute the line in the image space by using
    // y =           m           * x +      b
    // y = -cos(theta)/sin(theta)* x + p/sin(theta)
    double m, b, rho, theta;
    for (int r = 0; r < HOUGH_ROWS; r++) {
        for (int c  = 0; c < HOUGH_COLS; c++) {
            if (HA[r][c] >= threshold_value) {
                rho = r;
                theta = c*M_PI/99;

                // m will just equal -infinity if theta=0
                // and we don't care about the slope in that case anyways
                if (theta == 0) b = rho;
                else            b = rho/sin(theta);

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

enum _hw3_error printHoughPeaks(HoughPeakNode* head)
{
    //make sure there are more nodes than head node
    if (head->next == NULL) return E_NO_NODES;

    while (head->next != NULL) {
        head = head->next;
        printHoughPeak(head->m, head->b, head->rho, head->theta, head->intensity);
        if (head->next == NULL) break;
    }
    return E_SUCCESS;
}

enum _hw3_error printHoughLineEquations(HoughPeakNode* head)
{
    //make sure there are more nodes than head node
    if (head->next == NULL) return E_NO_NODES;

    while (head->next != NULL) {
        head = head->next;
        printHoughLineEquation(head->m, head->b);
        if (head->next == NULL) break;
    }
    return E_SUCCESS;
}

void printHoughPeak(double m, double b, double rho, double theta, uint8_t intensity)
{
    printf("rho:%.1lf\t theta:%.2lf\t intensity:%d, m:%.2lf\t  b:%lf\n", rho, theta, intensity, m, b);
}


void printHoughLineEquation(double m, double b)
{
    if (m == -1.0/0.0)
        printf("x=%lf\n",b);
    else
        printf("y=%lfx+%lf\n", m, b);

}


void drawLineFromSlopeAndIntercept(uint8_t **HAlines, double m, double b)
{
    //TODO: find x1 and y1
}

// Bresenham's line drawing algorithm
void drawLine(uint8_t **HAlines, int x0, int y0, int x1, int y1)
{
    int dx, dy, p, x, y;
    dx=x1-x0; dy=y1-y0;
    x=x0; y=y0; p=2*dy-dx;

    while(x<x1)
    {
        if(p>=0)
        {
            HAlines[x][y] = 255;
            y=y+1;
            p=p+2*dy-2*dx;
        }
        else
        {
            HAlines[x][y] = 255;
            p=p+2*dy;
        }
        x=x+1;
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

    // we are doing 1) (b)
    // 0 <= theta < pi
    // -n_rows <= p <= sqrt(r^2 + c^2)

    //setting N to n_cols seems good enough.
    //future could be (n_rows+n_cols)/2
    //int HA_rho_min = n_cols;
    //int HA_rho_min = (n_cols+n_rows)/2;
    int HA_rho_max = (int)sqrt(pow(n_rows,2)+pow(n_cols,2));
    int HA_rho_min = -HA_rho_max;

    int32_t r, c;
    int8_t HA_rho;
    double theta, p;
    bool valid_edge;
    //pragma omp parallelizes the OUTER 2 loops w/ collapse(2) argument
    //#pragma omp parallel for private(r, c, p, theta) shared(HA) collapse(2)
    for (r = n_rows-1; r >0; r--) {
        for (c = 0; c < n_cols; c++) {
            //valid_edge checks if the pixel is an edge or not
            valid_edge = (edge_map[r][c] > 100) ? true : false;
            if (valid_edge) {
             int theta_counter=0;
             for (theta = -M_PI/2; theta < M_PI/2; theta += M_PI/100) {
              //classic rho algorithm
              p = (double)r*cos(theta) + (double)c*sin(theta);

              // rho will be between 0 and 100 in the hough array,
              // but the rho values can be between -N and Nsqrt(2)
              // so in the case of the provided edge map we map
              // -256 -> 352
              // to
              // 0 -> 99
              // we calculate the scaled value as
              // new value = (old_val - old_min)/(old_max - old_min) * new_max
              HA_rho = (double)round(((p - (double)HA_rho_min)/((double)HA_rho_max - (double)HA_rho_min))*100.0);

              //printf("r:%d\tc:%d\ttheta: %lf\tp:%lf\n", r, c, theta, p);
              HA[HA_rho][theta_counter]++;
              theta_counter++;
             }
            }
        }
    }
    return;
}
