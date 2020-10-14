#pragma once
#include <stdint.h>
#include <math.h>
#include "pngio.h"

typedef enum {
    UINT8_T,
    INT32_T,
} TYPE;

/* hw4 error codes for debugging the program */
enum _hw4_error
{
    E_SUCCESS,
    E_FILE_NOT_FOUND,
    E_GENERIC_ERROR,
};

typedef enum _hw4_error error_t;

/* helper function to easily decode error message */
struct _errordesc{
    int code;
    char *message;
} errordesc[] = {
    { E_SUCCESS,  (char *)"No error" },
    { E_FILE_NOT_FOUND, (char *)"File not found" },
    { E_GENERIC_ERROR,  (char *)"Generic Error" },
};

void printError(error_t E, char *msg)
{
    printf("Error %d: %s %s\n", errordesc[E].code, errordesc[E].message ,msg);
}

void printHelp()
{
    printf("call the program as\n");
    printf("thresh [-t T] infile outfile\n");
    printf("\n");
    printf("T: optional user specified threshold 0 <= T <= 255;\n");
    printf(" default: T is automatically computed using Kittler's method\n");
    printf("\n");
    printf("infile: the input grayscale image\n");
    printf("\n");
    printf("outfile: the output bi-level image\n");
}

error_t analyzeImage();
error_t zeroPsuedo2DArray(void** array, int32_t n_rows, int32_t n_cols,
        TYPE type);

/* @brief P is the observed "probability distribution"
 *        P = h(t)/#numpixels
 * @param h histogram created as a single uint8_t array
 * @param t index of histogram
 * @return double
 */
double KittlerP(uint8_t *h, int t) {return  h[t] / (sizeof(*h) / sizeof(h[0]));}

/* @brief Kittler q1
 *       t
 *  q1 = Σ P(h,t)
 *      i=0
 * @param h histogram created as a single uint8_t array
 * @param t index of histogram
 * @return double
 */
double Kittlerq1(uint8_t *h, int t)
{
    double sum = 0;
    for (int i = 0; i <= t; i++) sum += KittlerP(h, t);
    return sum;
}

/* @brief Kittler q2
 *       255
 *  q2 = Σ P(h,t) = 1 - q1(h,t)
 *      i=t+1
 * @param h histogram created as a single uint8_t array
 * @param t index of histogram
 * @return double
 */
double Kittlerq2(uint8_t *h, int t) { return 1.0 - Kittlerq1(h, t); }

/* @brief Kittler u1
 *       t
 *  u1 = Σ i*P(h,t)/q1(h,t)
 *      i=0
 * @param h histogram created as a single uint8_t array
 * @param t index of histogram
 * @return double
 */
double Kittleru1(uint8_t *h, int t)
{
    double sum = 0;
    for (int i = 0; i <= t; i++) sum += i*(KittlerP(h,t))/Kittlerq1(h,t);
    return sum;
}

/* @brief Kittler u2
 *      255
 *  u2 = Σ i*P(h,t)/q2(h,t)
 *     i=t+1
 * @param h histogram created as a single uint8_t array
 * @param t index of histogram
 * @return double
 */
double Kittleru2(uint8_t *h, int t)
{
    double sum = 0;
    for (int i = t + 1; i <= 255; i++) sum += i*(KittlerP(h,t))/Kittlerq2(h,t);
    return sum;
}

/* @brief Kittler var1
 *       t
 *  var1 = Σ ([i-u1(h,t)]^2 * P(h,t) * q1(h,t))
 *      i=0
 * @param h histogram created as a single uint8_t array
 * @param t index of histogram
 * @return double
 */
double Kittlervar1(uint8_t *h, int t)
{
    double sum = 0;
    for (int i = 0; i <= t; i++)
        sum += pow(i-Kittleru1(h,t),2)*(KittlerP(h,t))/Kittlerq1(h,t);
    return sum;
}

/* @brief Kittler var2
 *      255
 *  var2 = Σ ([i-u2(h,t)]^2 * P(h,t) * q2(h,t))
 *     i=t+1
 * @param h histogram created as a single uint8_t array
 * @param t index of histogram
 * @return double
 */
double Kittlervar2(uint8_t *h, int t)
{
    double sum = 0;
    for (int i = t + 1; i <= 255; i++)
        sum += pow(i-Kittleru2(h,t),2)*(KittlerP(h,t))/Kittlerq2(h,t);
    return sum;
}

/* @brief f is representation of histogram as sum of 2 gaussian functions
 *   f(t) = q1*gauss1 + q2*gauss2
 *
 * @param h histogram as a pointer to uint8_t array
 * @param t index of histogram gaussian representation
 *
 * @return double
 */
double Kittlerf(uint8_t *h, int t)
{
    double g1 = Kittlerq1(h,t)/( Kittlervar1(h,t)*sqrt(2*M_PI) ) *
        pow( M_E, -1.0 * pow( (t-Kittleru1(h,t)),2)/(2*Kittlervar1(h,t) ) );

    double g2 = Kittlerq2(h,t)/( Kittlervar2(h,t)*sqrt(2*M_PI) ) *
        pow( M_E, -1.0 * pow( (t-Kittleru2(h,t)),2)/(2*Kittlervar2(h,t) ) );

    return g1 + g2;

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
error_t convert2DPseudoArrayToHistogram(uint8_t **grayscale, int32_t n_rows,
        int32_t n_cols, uint8_t *Histogram)
{
    uint8_t histogram[n_rows * n_cols];
    int i, j, k =0 ;
    for (i=0;i<n_rows;i++) {
        for (j=0;j<n_cols;j++) {
            histogram[k] = grayscale[i][j];
            k++;
        }
    }
    Histogram = histogram;
    return E_SUCCESS;
}

convertHistogramToPseudo2DArray(uint8_t *Histogram, in32_t n_rows,
        int32_t n_cols, uint8_t **out)
{




}




/* @brief Kittler and Illingworth method to determine optimal threshold value
 *
 * @param histogram as pointer to a uint8_t array
 * @param thresh pointer to int8_t that threshold value should be stored into
 *
 * @return error_t
 */
error_t KittlerIllingworthThresholding(uint8_t *histogram, int8_t *thresh);


/* @brief Recursive Update Formula, this is the formula that determines
 * each value in the histogram based on a threshold
 *
 * @param h histogram as a pointer to a uint8_t array
 * @param t int32_t threshold value
 *
 * @return error_t
 */
error_t RecursiveUpdateFormula(uint8_t *h, int32_t t);


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
error_t zeroPsuedo2DArray(void** array, int32_t n_rows, int32_t n_cols,
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
    return E_SUCCESS;
}

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

