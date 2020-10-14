#include <stdio.h>
#include <stdlib.h>
#include "pngio.h"
#include "pngOpen.c"
#include "pngReadHdr.c"
#include "pngErrorHandler.c"
#include "thresh.h"
#include "matfree.c"
#include "pngReadRow.c"
#include "padarray.c"
#include <limits.h>
#include <stdbool.h>
#include <float.h>
#include <math.h>

#define HOUGH_ROWS 100
#define HOUGH_COLS 100

int8_t threshold_value = -1; // user defined or Kittler's method

char *INPUT_FILE = "";
char *OUTPUT_FILE = "";


void printHelp()
{
    //TODO: print help menu
}

/*
 * call the program as
 * thresh [-t T] infile outfile
 *
 * T: optional user specified threshold 0 <= T <= 255;
 *  default: T is automatically computed using Kittler's method
 *
 * infile: the input grayscale image
 *
 * outfile: the output bi-level image
 */
int main(int argc,char* argv[]) {
    if (argc==1) { //just calling the program by itself
        printf("Error: provide correct amount of inputs");
        printHelp();
    }
    if (argc==2) { // calling program with a single param
        printf("Error: provide correct amount of inputs");
        printHelp();
    }
    if (argc==3) { // calling program with 2 params
        INPUT_FILE = argv[2]; //TODO: error check
        OUTPUT_FILE = argv[3]; //TODO: error check
        analyzeImage();
    }
    if (argc==4) { //not enough params
        printf("Error: provide correct amount of inputs");
        printHelp();
    }
    if (argc==5) { // defining threshold
        threshold_value = atoi(argv[2]); //TODO: error check
        INPUT_FILE = argv[3]; //TODO: error check
        OUTPUT_FILE = argv[4]; //TODO: error check
        analyzeImage();
    }
    if (argc>5) { //too many params
        printf("Error: provide correct amount of inputs");
        printHelp();
    }
exit:
    return 0;
}

typedef enum {
    UINT8_T,
    INT32_T,
} TYPE;


//_hw4_error zeroPsuedo2DArray(void** array, int32_t n_rows, int32_t n_cols, \
//        TYPE type)
//{
//    int32_t r, c;
//    //zero out Hough Array
//    for (r = 0; r < n_rows; r++) {
//        for (c = 0; c < n_cols; c++) {
//            switch(type) {
//                case INT32_T:
//                    ((int32_t**)array)[r][c] = 0;
//                    break;
//                case UINT8_T:
//                    ((uint8_t **)array)[r][c] = 0;
//                    break;
//            }
//        }
//    }
//    return E_SUCCESS;
//}

double P(uint8_t *h, int t)
{
    // P(i) = h[i]/#pixels
    return  h[t] / (sizeof(*h) / sizeof(h[0]));
}

double q1(uint8_t *h, int t)
{
    double sum = 0;
    for (int i = 0; i <= t; i++)
        sum += P(h, t);
    return sum;
}

double q2(uint8_t *h, int t) { return (double)1 - q1(h, t); }

double u1(uint8_t *h, int t)
{
    //      t
    // u1 = Σ iP(i)/q1(t)
    //      i=0
    double sum = 0;
    for (int i = 0; i <= t; i++)
        sum += i*(P(h,t))/q1(h,t);
    return sum;
}

double u2(uint8_t *h, int t)
{
    //      255
    // u2 = Σ i*P(i)/q2(t)
    //      i=t+1
    double sum = 0;
    for (int i = t + 1; i <= 255; i++)
        sum += i*(P(h,t))/q2(h,t);
    return sum;
}


double var1(uint8_t *h, int t)
{
    //        t
    // var1 = Σ ([i-u1(t)]^2 * P(i) * q1(t))
    //        i=0
    double sum = 0;
    for (int i = 0; i <= t; i++)
        sum += pow(i-u1(h,t),2)*(P(h,t))/q1(h,t);
    return sum;
}

double var2(uint8_t *h, int t)
{
    //        255
    // var2 = Σ ([i-u2(t)]^2 * P(i) * q2(t))
    //        i=t+1
    double sum = 0;
    for (int i = t + 1; i <= 255; i++)
        sum += pow(i-u2(h,t),2)*(P(h,t))/q2(h,t);
    return sum;
}

double f(uint8_t *h, int t)
{
    // f(t) = q1*gauss1 + q2*gauss2
    double g1 = q1(h,t)/( var1(h,t)*sqrt(2*M_PI) ) *
        pow( M_E, -1.0 * pow( (t-u1(h,t)),2)/(2*var1(h,t) ) );

    double g2 = q2(h,t)/( var2(h,t)*sqrt(2*M_PI) ) *
        pow( M_E, -1.0 * pow( (t-u2(h,t)),2)/(2*var2(h,t) ) );

    return g1 + g2;

}

uint8_t * convert2DPseudoArrayToHistogram(uint8_t **grayscale, int32_t n_rows,
        int32_t n_cols)
{
    uint8_t Histogram[n_rows * n_cols];
    int i, j, k =0 ;
    for (i=0;i<n_rows;i++) {
        for (j=0;j<n_cols;j++) {
            Histogram[k] = grayscale[i][j];
            k++;
        }
    }
    return Histogram;
}


// Kittler and Illingworth's Kullback information minimization approach.
int32_t KittlerIllingworthThresholding(uint8_t **grayscale, int32_t n_rows,
        int32_t n_cols)
{
    double q1, q2, u1, u2, var1, var2;
    int32_t T = -1;
    int i, j;

    // step 1) compute histogram, h(i)
    // convert grayscale to histogram
    uint8_t *Histogram;
    Histogram = convert2DPseudoArrayToHistogram(grayscale, n_rows, n_cols);

    // Step 2) Normalize: P(i) = h(i)/#pixels
    // done in function above (:

    // step 3) for each t, compute H(t)
    double min = -1, H, sum;
    for (int t = 0; t <= 255; t++) {
        sum = 0;
        for (i = 0; i < 255;i++) {
            sum += -(P(Histogram,t)*log(f(Histogram,t)));
        }
        if ( H < min) T  = t;
    }

    // step 4) return t giving the smalled H(t)
    return T;
}

double RecursiveUpdateFormula(double q1, double q2, double mu1prev,
        double mu2prev, double *P, double q1prev, double var1prev, int32_t t,
        double mu, double q2prev, double var2prev)
{
    double mu1, var1, mu2, var2, H;
    mu1 = (q1prev*mu1prev+t*P[t])/q1;

    var1 = (q1prev*(var1prev+(mu1prev-mu1)*(mu1prev-mu1)) +P[t]*(t-mu1)*(t-mu1))
        /q1;

    mu2 = (mu-q1*mu1)/q2;

    var2 = (q2prev*(var2prev+(mu2prev-mu2)*(mu2prev-mu2)) -P[t]*(t-mu2)*(t-mu2))
        /q2;

    H = (q1*log(var1)+q2*log(var2))/2-q1*log(q1)-q2*log(q2);

    return H;
}


void analyzeImage()
{
    int32_t c, r, n_cols, n_rows;
    //initialize input picture
    PNGFILE *pngfile = pngOpen(INPUT_FILE, "r");
    pngReadHdr(pngfile, &n_rows, &n_cols);
    printf("image is %d rows by %d cols\n", n_rows, n_cols);

    //initial pseudo 2D arrays
    uint8_t **png_raw = matalloc(n_rows, n_cols, 0, 0, sizeof(uint8_t));
    int32_t **HA      = matalloc(HOUGH_ROWS, HOUGH_COLS, 0, 0, sizeof(int32_t));
    uint8_t **HAout   = matalloc(HOUGH_ROWS, HOUGH_COLS, 0, 0, sizeof(uint8_t));

    //store entire image into memory
    for (r = 0; r < n_rows; r++) pngReadRow(pngfile, png_raw[r]);

    //zeroPsuedo2DArray(HA, HOUGH_ROWS, HOUGH_COLS, INT32_T);


    goto cleanup;
cleanup:
    matfree(png_raw);
    matfree(HA);
    matfree(HAout);
    pngClose(pngfile);
}
