#ifndef THRESH_H
#define THRESH_H

#include <stdint.h>
#include <math.h>
#include <stdio.h>
// include all of the libdip header and source files
#include "../libdipsrc/pngio.h"
#include "../libdipsrc/matfree.c"
#include "../libdipsrc/pngReadRow.c"
#include "../libdipsrc/padarray.c"
#include "../libdipsrc/pngErrorHandler.c"
#include "../libdipsrc/pngOpen.c"
#include "../libdipsrc/pngReadHdr.c"

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
    E_ARRAY_SIZE_MISMATCH,
    E_NOT_IMPLEMENTED,
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
    { E_ARRAY_SIZE_MISMATCH,  (char *)"Array sizes do not match" },
    { E_NOT_IMPLEMENTED,  (char *)"Not implemented yet" },
};

void printError(error_t E, char *msg)
{
    printf("Error %d: %s %s\n", errordesc[E].code, errordesc[E].message ,msg);
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
double KittlerP(uint8_t *h, int t) {
    uint8_t hval = h[t];
    double sizeArray = (double)sizeof(*h);
    double sizeElement = (double)sizeof(h[0]);

    double numPixels = round(sizeArray / sizeElement);

    //printf("KittlerP <= hval/numPixels t: %d \thval: %d \tnumPixels: %lu\n", t, hval, numPixels);
    double P = ((double)hval / numPixels);

    return P;
}

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
    double sum = 0, P;

    P = KittlerP(h, t);

    for (int i = 0; i <= t; i++) sum += P;

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
    double sum = 0, P, q1;

    P = KittlerP(h,t);
    q1 = Kittlerq1(h,t);

    for (int i = 0; i <= t; i++) {
        sum += (i*P)/q1;
    }
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
    double sum = 0, P, q2;

    P  = KittlerP(h,t);
    q2 = Kittlerq2(h,t);

    for (int i = t + 1; i <= 255; i++) {
        sum += i*P/q2;
    }
    return sum;
}

/* @brief Kittler var1
 *       t
 *  var1 = Σ ([i-u1(t)]^2 * P(t) * q1(t))
 *      i=0
 * @param h histogram created as a single uint8_t array
 * @param t index of histogram
 * @return double
 */
double Kittlervar1(uint8_t *h, int t)
{
    double sum = 0, P, u1, q1;
    P  = KittlerP(h,t);
    u1 = Kittleru1(h,t);
    q1 = Kittlerq1(h,t);
    for (int i = 0; i <= t; i++) {
        sum += pow(i-u1,2) * P * q1;
    }
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
    double sum = 0, u2, P, q2;
    P  = KittlerP(h,t);
    u2 = Kittleru2(h,t);
    q2 = Kittlerq2(h,t);
    for (int i = t + 1; i <= 255; i++) {
        sum += pow(i-u2,2)*(P) * q2;
    }
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
    double q1   = Kittlerq1(h,t);
    double var1 = Kittlervar1(h,t);
    double u1   = Kittleru1(h,t);

    //printf("Kittlerf q1:%lf \tvar1:%lf \tu1:%lf\n",q1, var1, u1);

    double g1 = q1/( var1*sqrt(2*M_PI) ) *
        pow( M_E, -1.0 * pow( (t-u1),2)/(2*var1 ) );

    double q2   = Kittlerq2(h,t);
    double var2 = Kittlervar2(h,t);
    double u2   = Kittleru2(h,t);

    //printf("Kittlerf q2:%lf \tvar2:%lf \tu2:%lf\n",q2, var2, u2);

    double g2 = q2/( var2*sqrt(2*M_PI) ) *
        pow( M_E, -1.0 * pow( (t-u2),2)/(2*var2) );

    //printf("g1: %lf, g2:%lf\n",g1,g2);
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
    int i, j, k = 0;
    for (i=0;i<n_rows;i++) {
        for (j=0;j<n_cols;j++) {
            //printf("grayscale[%d][%d]: %d\n", i, j, grayscale[i][j]);
            histogram[k] = grayscale[i][j];
            k++;
        }
    }
    Histogram = histogram;
    return E_SUCCESS;
}

error_t convertHistogramToPseudo2DArray(uint8_t *Histogram, int32_t n_rows,
        int32_t n_cols, uint8_t **out)
{
    return E_NOT_IMPLEMENTED;
}




/* @brief Kittler and Illingworth method to determine optimal threshold value
 *
 * @param histogram as pointer to a uint8_t array
 * @param thresh pointer to int that threshold value should be stored into
 *
 * @return error_t
 */
error_t KittlerIllingworthThresholding(uint8_t *histogram, int *thresh);


/* @brief Recursive Update Formula, this is the formula that determines
 * each value in the histogram based on a threshold
 *
 * @param h histogram as a pointer to a uint8_t array
 * @param t int32_t threshold value
 *
 * @return error_t
 */
error_t RecursiveUpdateFormula(uint8_t *h, int32_t t, double *Hvalues);


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


double Rq1(uint8_t *h, int8_t t)
{
    //q1(0) = P(0)
    if ( t == 0 ) return KittlerP(h, 0);

    //q1(t) = q1(t-1) + P(t)
    return Rq1(h, t-1) + KittlerP(h,t);
}

double Rq2(uint8_t *h, int8_t t)
{
    //q2(t) = 1 - q1(t)
    return (1 - Rq1(h,t));
}

double Ru1(uint8_t *h, int8_t t)
{
    // u1(0) = 0
    if ( t == 0 ) return 0;

    double q1prev = Rq1(h,t-1);
    double u1prev = Ru1(h,t-1);
    double P = KittlerP(h,t);
    double q1 = Rq1(h,t);

    // u1(t) = (q1(t-1)*u1(t-1) + t*P(t) )/ q1(t)
    return (q1prev*u1prev + t*P)/q1;
}

//      255
//  u =  Σ i*P(h,t)
//      i=0
double Ru(uint8_t *h, int t)
{
    double sum = 0, P;

    P = KittlerP(h, t);

    for (int i = 0; i <= 255; i++) sum += i*P;

    return sum;
}

double Ru2(uint8_t *h, int t)
{
    double u = Ru(h,t);
    double q1 = Rq1(h,t);
    double q2 = Rq2(h,t);
    double u1 = Ru1(h,t);

    double Ru2 = 0;
    // u2(0) = u/q2(0)
    // u2(t) = (u-q1(t)u1(t))/q2(t)
    if (t == 0) { Ru2 = u/q2; }
    else { Ru2 = (u-q1*u1)/q2; }

    return Ru2;
}

double Rvar2(uint8_t *h, int t)
{
    int i=0;
    double sum=0, var2=0;

    double u2       = Ru2(h,t);
    double P        = KittlerP(h,t);
    double q2       = Rq2(h,t);
    double q2prev   = Rq2(h,t-1);
    double var2prev = Rvar2(h,t-1);
    double u2prev   = Ru2(h,t-1);

    //         255
    //  var2(0) = Σ [i - u2(0)]^2 * P(i)/q2(0)
    //         i=1
    if ( t == 0 ) {
        sum = 0;
        for (i = 1; i <= 255; i++) {
            sum += (pow(i-u2,2) * P/q2);
        }
        return sum;
    }

    // var2(t) = (1/q2) * (q2prev
    //           {var2prev + [u2prev - u2]^2}
    //           - P(t)*[t-u2prev]^2)
    var2 = (1.0/q2)*(q2prev*(var2prev+pow(-u2,2)) - P*pow(t-u2,2));
    return var2;
}

double Rvar1(uint8_t *h, int t)
{
    double var1 = 0;
    double u1       = Ru1(h,t);
    double P        = KittlerP(h,t);
    double q1       = Rq1(h,t);
    double q1prev   = Rq1(h,t-1);
    double var1prev = Rvar1(h,t-1);
    double u1prev   = Ru1(h,t-1);

    //var1(0) = 0
    if ( t == 0) return 0;

    var1 = (1.0/q1)*(q1prev*(var1prev+pow(-u1,2)) - P*pow(t-u1,2));
    return var1;
}

#endif // THRESH_H
