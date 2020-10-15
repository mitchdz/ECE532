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

int threshold_value = -1; // user defined or Kittler's method

char INPUT_FILE[50];
char OUTPUT_FILE[50];

int main(int argc,char* argv[]) {
    error_t error;
    if (argc==3) { // calling program with 2 params
        strcpy(INPUT_FILE, argv[1]);//TODO: error check
        strcpy(OUTPUT_FILE, argv[2]);//TODO: error check
        printf("\n");
        printf(" input file : %s\n", INPUT_FILE);
        printf("output file : %s\n", OUTPUT_FILE);
        printf("  threshold : not defined\n");
        printf("\n");
        error = analyzeImage();
    }
    else if (argc==5) { // defining threshold
        threshold_value = atoi(argv[1]); //TODO: error check
        strcpy(INPUT_FILE, argv[3]);//TODO: error check
        strcpy(OUTPUT_FILE, argv[4]);//TODO: error check
        printf("\n");
        printf(" input file : %s\n", INPUT_FILE);
        printf("output file : %s\n", OUTPUT_FILE);
        printf("  threshold : not defined\n");
        printf("\n");
        error = analyzeImage();
    }
    else { //too many params
        printf("Error: provide correct amount of inputs");
        printHelp();
        goto exit;
    }
    printError(error, "");

exit:
    return 0;
}

error_t analyzeImage()
{
    int32_t r, n_cols, n_rows;
    error_t error;
    // initialize input picture
    PNGFILE *pngfile = pngOpen(INPUT_FILE, "r");
    pngReadHdr(pngfile, &n_rows, &n_cols);
    printf("image is %d rows by %d cols\n", n_rows, n_cols);

    // store entire image into memory
    uint8_t **grayscale = matalloc(n_rows, n_cols, 0, 0, sizeof(uint8_t));
    for (r = 0; r < n_rows; r++) pngReadRow(pngfile, grayscale[r]);

    double Hvalues[n_rows*n_cols];

    // convert image to histogram
    uint8_t Histogram[n_rows*n_cols];
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
        printf("Determined threshold: %d\n", threshold_value);
    }

    // Recursive update formula to find H values
    error=RecursiveUpdateFormula(Histogram, threshold_value, Hvalues);
    if (error != E_SUCCESS) {
        printError(error, "Could not execute recursive update formula");
        goto cleanup;
    }
    printf("Gray\tH Value\n");
    printf("----\t------\n");
    for (int i = 27; i <= 36; i++)
        printf("%d\t\t%lf\n",i, Hvalues[i]);
    printf("...\n");
    for (int i = 137; i <= 146; i++)
        printf("%d\t\t%lf\n",i, Hvalues[i]);

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


error_t KittlerIllingworthThresholding(uint8_t *histogram, int *thresh)
{
    // step 1) compute histogram, h(i)
    // converted in main (:

    // Step 2) Normalize: P(i) = h(i)/#pixels
    // done in helper function (:

    int32_t T = 255, i;
    double min = -1, sum, P, f;
    // step 3) for each t, compute H(t) where H(t) is defined as follows:
    //           t
    //  H(t) = - Σ P(h,t)log(f(h,t))
    //          i=0
    for (int t = 0; t <= 255; t++) {
        sum = 0;
        P = KittlerP(histogram,t);
        f = Kittlerf(histogram,t);
        for (i = 0; i < 255;i++) {
            sum += (P*log(f));
        }
        if ( (-1.0 * sum) < min) T  = t;
    }

    // step 4) return t giving the smallest H(t)
    *thresh = T;
    return E_SUCCESS;
}

error_t RecursiveUpdateFormula(uint8_t *h, int32_t t, double *Hvalues)
{
    double var1, var2, H;
    int numPixels = sizeof(*h) / sizeof(h[0]);

    // checks if Hvalues array is same size as h
    if (numPixels != sizeof(*Hvalues) / sizeof(Hvalues[0]))
            return E_ARRAY_SIZE_MISMATCH;

    var1 = Rvar1(h,t);
    var2 = Rvar2(h,t);
    // recursive update function as shown in homework pdf
    for (int i=1; i < numPixels-1; i++) {
        H = ( Rq1(h,t)*log(var1) + Rq2(h,t)*log(var2) ) / 2
            - Rq1(h,t)*log(Rq1(h,t)) - Rq2(h,t)*log(Rq2(h,t));

        Hvalues[i] = H;
    }

    return E_SUCCESS;
}


