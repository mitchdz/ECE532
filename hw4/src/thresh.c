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

int8_t threshold_value = -1; // user defined or Kittler's method

char *INPUT_FILE = "";
char *OUTPUT_FILE = "";

int main(int argc,char* argv[]) {
    error_t error;
    if (argc==3) { // calling program with 2 params
        INPUT_FILE = argv[2]; //TODO: error check
        OUTPUT_FILE = argv[3]; //TODO: error check
        printf("input file: %s\n", INPUT_FILE);
        printf("output file: %s\n", OUTPUT_FILE);
        printf("threshold: not defined\n");
        error = analyzeImage();
    }
    else if (argc==5) { // defining threshold
        threshold_value = atoi(argv[2]); //TODO: error check
        INPUT_FILE = argv[2]; //TODO: error check
        OUTPUT_FILE = argv[3]; //TODO: error check
        printf("input file: %s\n", INPUT_FILE);
        printf("output file: %s\n", OUTPUT_FILE);
        printf("threshold: %d\n", threshold_value);
        error = analyzeImage();
    }
    else { //too many params
        printf("Error: provide correct amount of inputs");
        printHelp();
        goto exit;
    }

exit:
    return 0;
}

error_t analyzeImage()
{
    int32_t c, r, n_cols, n_rows;
    error_t error;
    // initialize input picture
    PNGFILE *pngfile = pngOpen(INPUT_FILE, "r");
    pngReadHdr(pngfile, &n_rows, &n_cols);
    printf("image is %d rows by %d cols\n", n_rows, n_cols);

    // store entire image into memory
    uint8_t **grayscale = matalloc(n_rows, n_cols, 0, 0, sizeof(uint8_t));
    for (r = 0; r < n_rows; r++) pngReadRow(pngfile, grayscale[r]);

    int8_t x[n_rows*n_cols];

    // convert image to histogram
    uint8_t *Histogram;
    error = convert2DPseudoArrayToHistogram(grayscale, n_rows, n_cols,
            Histogram);
    if (error != E_SUCCESS) {
        printError(error, "Could not convert picture to histogram");
        goto cleanup;
    }

    // if no threshold defined, determine threshold
    if (threshold_value == -1) {
        error=KittlerIllingworthThresholding(Histogram, &threshold_value);
        if (error != E_SUCCESS) {
            printError(error, "Error when trying to find thresholding value");
            goto cleanup;
        }
    }

    // Recursive update formula to find x
    error=RecursiveUpdateFormula(Histogram, threshold_value);
    if (error != E_SUCCESS) {
        printError(error, "Could not execute recursive update formula");
        goto cleanup;
    }

    // convert resulting array to grayscale
    error=convertHistogramToPseudo2DArray(Histogram, n_rows,n_cols, grayscale);
    if (error != E_SUCCESS) {
        printError(error, "Could not convert histogram to pseudo 2D array");
        goto cleanup;
    }
    for (int r = 0; r < n_rows; r++) {
        for (int c = 0; c < n_cols; c++) {
        grayscale[r][c] = (grayscale[r][c] >= threshold_value) ? 255 : 0;
        }
    }

    //write output grayscale to file
    writePNG(grayscale, OUTPUT_FILE, n_rows, n_cols);

    error = E_SUCCESS;
cleanup:
    if (grayscale != NULL) matfree(grayscale);
    if (pngfile != NULL) pngClose(pngfile);
    return error;
}


error_t KittlerIllingworthThresholding(uint8_t *histogram, int8_t *thresh)
{
    // step 1) compute histogram, h(i)
    // converted in main (:

    // Step 2) Normalize: P(i) = h(i)/#pixels
    // done in helper function (:

    // step 3) for each t, compute H(t) where H(t) is defined as follows:
    //           t
    //  H(t) = - Σ P(h,t)log(f(h,t))
    //          i=0
    int32_t T = -1, i, j;
    double min = -1, sum;
    for (int t = 0; t <= 255; t++) {
        sum = 0;
        for (i = 0; i < 255;i++)
            sum += (KittlerP(histogram,t)*log(Kittlerf(histogram,t)));
        if ( (-1.0 * sum) < min) T  = t;
    }

    // step 4) return t giving the smalled H(t)
    *thresh = T;
    return E_SUCCESS;
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
    return 1 - Rq1(h,t);
}

double Ru1(uint8_t *h, int8_t t)
{
    // u1(0) = 0
    if ( t == 0 ) return 0;

    // u1(t) = q1(t-1)*u1(t-1) + t*P(t) / q1(t)
    return (Rq1(h,t-1)*Ru1(h,t-1) + t*KittlerP(h,t))/Rq1(h,t);
}

//      255
//  u =  Σ i*P(h,t)
//      i=0
double Ru(uint8_t *h)
{
    double sum = 0;
    for (int i = 0; i <= 255; i++)
        sum += i*KittlerP(h,i);
    return sum;
}

double Ru2(uint8_t *h, int8_t t)
{
    // u2(0) = u/q2(0)
    if (t == 0) return (Ru(h)/Rq2(h,0));

    //u2(t) = (u-q1(t)u1(t))/q2(t)
    return (Ru(h)-Rq1(h,t)*Ru1(h,t))/Rq2(h,t);
}

double Rvar1(uint8_t *h, int8_t t)
{
    //var1(0) = 0
    if ( t == 0) return 0;

    //var1(t) = a lot of stuff
    return (1.0/Rq1(h,t)) * ( (Rq1(h,t-1) \
            * ( Rvar1(h,t-1) + pow(Ru1(h,t-1)-Ru1(h,t),2)) \
            + KittlerP(h,t) * pow((t-Ru1(h,t)),2)) );
}

double Rvar2(uint8_t *h, int8_t t)
{
    int i;
    double sum;
    //         255
    //  var2(0) = Σ [i - u2(0)]^2 * P(i)/q2(0)
    //         i=1
    if ( t == 0 ) {
        sum = 0;
        for (i = 1; i <= 255; i++) {
            sum += (pow(i-Ru2(h,0),2) * KittlerP(h,t)/Rq2(h,0));
        }
        return sum;
    }

    // var2(t) = a lot of annoying stuff to type out
    return (1.0/Rq2(h,t)) * ( (Rq2(h,t-1) \
            * ( Rvar2(h,t-1) + pow(Ru2(h,t-1)-Ru2(h,t),2)) \
            + KittlerP(h,t) * pow((t-Ru2(h,t)),2)) );
}

error_t RecursiveUpdateFormula(uint8_t *h, int32_t t)
{
    double mu1, mu2, q1, q2, var1, var2, H;
    double H;

    double q1prev   =   q1(h,t-1);
    double mu1prev  =   u1(h,t-1);
    double var1prev = var1(h,t-1);


    mu1 = ( q1prev*mu1prev + t*P(h,t) ) / q1(h,t);

    var1 = (q1prev*(var1prev+(mu1prev-mu1)*(mu1prev-mu1)) +P[t]*(t-mu1)*(t-mu1))
        / q1;

    mu2 = (mu-q1*mu1)/q2;

    var2 = (q2prev*(var2prev+(mu2prev-mu2)*(mu2prev-mu2)) -P[t]*(t-mu2)*(t-mu2))
        / q2;

    H = (q1*log(var1)+q2*log(var2))/2-q1*log(q1)-q2*log(q2);

    return E_SUCCESS;
}


