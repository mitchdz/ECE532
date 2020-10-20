#include "thresh.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <float.h>
#include <math.h>

int threshold_value = -1; // user defined or Kittler's method
char INPUT_FILE[50];
char OUTPUT_FILE[50];
#define ARRAY_SIZE 256

void printHelp()
{
    printf("USAGE\n");
    printf("\tthresh [-t T] infile outfile\n");
    printf("\n");
    printf("\tT: optional user specified threshold 0 <= T <= 255;\n");
    printf("\t\tdefault: T is automatically computed using Kittler's method\n");
    printf("\tinfile: the input grayscale image\n");
    printf("\toutfile: the output bi-level image\n");
    printf("\n");
    printf("DESCRIPTION\n");
    printf("\tIf a fixed threshold is not specified, then it is automatically \
    computed by minimizing the kullback information measure. Specifically, \
    the image is thresholded as follows:\n");
    printf("\t\ty[n] = 255 if x[n] >= T\n");
    printf("\t\t       0   if x[n] <  T\n");
}

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
        error = thresholdPngWithKittler(INPUT_FILE);
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
        error = thresholdPngWithKittler(INPUT_FILE);
    }
    else { //too many params
        printf("Error: provide correct amount of inputs\n\n");
        printHelp();
        goto exit;
    }
    printError(error, "");

exit:
    return 0;
}

void printhw4HValues(double Hvalues[256])
{
    printf("H Values for address.png Image\n");
    printf("Gray\tH Value\n");
    printf("----\t------\n");
    for (int i = 27; i <= 36; i++)
        printf("%d\t\t%lf\n",i, Hvalues[i]);
    printf("\n");
    printf("...\n");
    printf("\n");
    for (int i = 137; i <= 146; i++)
        printf("%d\t\t%lf\n",i, Hvalues[i]);
}

error_t thresholdPngWithKittler(char *input_file)
{
    int32_t r, n_cols, n_rows;
    error_t error;
    // initialize input picture
    PNGFILE *pngfile = pngOpen(input_file, "r");
    pngReadHdr(pngfile, &n_rows, &n_cols);
    printf("image is %d rows by %d cols\n", n_rows, n_cols);

    // store entire image into memory
    uint8_t **grayscale = matalloc(n_rows, n_cols, 0, 0, sizeof(uint8_t));
    for (r = 0; r < n_rows; r++) pngReadRow(pngfile, grayscale[r]);

    double Hvalues[256];

    // convert image to histogram
    uint8_t Histogram[256];
    error = convert2DPseudoArrayToHistogram(grayscale, n_rows, n_cols,
            Histogram);
    if (error != E_SUCCESS) {
        printError(error, "Could not convert picture to histogram");
        goto cleanup;
    }

    // if no threshold defined, determine threshold
    if (threshold_value == -1) {
        error=RecursiveUpdateFormula(Histogram, &threshold_value, Hvalues);
        if (error != E_SUCCESS) {
            printError(error, "Error when trying to find thresholding value");
            goto cleanup;
        }
        printf("Determined threshold: %d\n", threshold_value);
    }

    printf("\n");
    printhw4HValues(Hvalues);

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

error_t RecursiveUpdateFormula(uint8_t *h, int *threshold, double *Hvalues)
{
    double H = 0, mu = 0;
    double q1prev=0, var1prev=0, mu1prev=0, mu1=0, mu2=0, q1=0, q2=0;
    double q2prev=0, var2prev=0, mu2prev=0, var1=0, var2=0;
    int i = 0, first = -1, last = -1, t = 0;

    // Find index before first-occupied histogram bin
    // note, can not be index 0
    for (i = 0; i < 255; i++) {
        if (h[i+1] != 0) {
            first = i;
            goto findLast;
        }
    }
findLast:
    // Find index of last-occupied histogram bin
    for (i = 255; i > 1; i--) {
        if (h[i-1] != 0) {
            last = i;
            goto calcArraySize;
        }
    }
calcArraySize:

    if (first == -1 || last == -1)
        return E_RECURSIVE_FIRST_LAST_NOT_FOUND;

    double q1vals   [ARRAY_SIZE] = {0}, 
           q2vals   [ARRAY_SIZE] = {0}, 
           mu1vals  [ARRAY_SIZE] = {0},
           mu2vals  [ARRAY_SIZE] = {0}, 
           var1vals [ARRAY_SIZE] = {0}, 
           var2vals [ARRAY_SIZE] = {0},
           P        [ARRAY_SIZE] = {0};

    //explicitly zero out Hvalues if not done before
    for (t = 0; t < 256;t++) Hvalues[t] = 0;

    //double numGrays=last+1-first+1;
    double numGrays=256.0;
    //calculate P vals
    for (t = first; t < last; t++) {

        P[t] = (h[t]/numGrays)/100.0; // TODO: do we still divide by 256, or array size?
    }

    //calculate q1 vals // note: q1[0] == 0
    q1vals[first] = 0;
    for (t = first+1; t < last; t++) {
        q1vals[t] = q1vals[t-1] + P[t];
    }

    //calculate q2 vals // note: q2[0] == 0
    q2vals[first] = 0;
    for (t = first+1; t < last; t++) {
        q2vals[t] = 1 - q1vals[t];  
    }

    //calculate mu1 vals // note: mu1[0] == 0
    mu1vals[first] = 0;
    for (t = first+1; t < last; t++) {            
        q1prev = q1vals[t-1];
        mu1prev = mu1vals[t-1];
        q1 = q1vals[t];
        mu1vals[t] = (q1prev*mu1prev+t*P[t])/q1; 
    }

    //calculate mu
    for (t = first; t < last; t++) {
        mu+=t*P[t];
    }


    //calcualte mu2 vals
    mu2vals[0] = mu/q2vals[0];
    for (t = first+1; t < last; t++) {
        q1 = q1vals[t];
        q2 = q2vals[t];
        mu1 = mu1vals[t];

        mu2vals[t] = (mu-q1*mu1)/q2; 
    }

    //calculate var1 vals
    for (t = first+1; t < last; t++) {
        q1prev = q1vals[t-1];
        var1prev = var1vals[t-1];
        mu1prev = mu1vals[t-1];
        mu1 = mu1vals[t];
        q1 = q1vals[t];

        var1vals[t] = (q1prev*(var1prev+(mu1prev-mu1)*(mu1prev-mu1)) +P[t]*(t-mu1)*(t-mu1))/q1; 
    }

    //calculate var2 vals
    for (t = first+1; t < last; t++) {
        q2prev   = q2vals[t-1];
        var2prev = var2vals[t-1];
        mu2prev  = mu2vals[t-1];
        mu2      = mu2vals[t];
        q2       = q2vals[t];

        var2vals[t] = (q2prev*(var2prev+(mu2prev-mu2)*(mu2prev-mu2)) -P[t]*(t-mu2)*(t-mu2))/q2; 
    }

    double determinedThreshold = DBL_MAX;
    double minimum = DBL_MAX;
    // recursive update function as shown in homework pdf
    for (int t=first+2; t <= last-2; t++) {
        q1 = q1vals[t];
        q2 = q2vals[t];
        var1 = var1vals[t];
        var2 = var2vals[t];

        H = (q1*log(var1)+q2*log(var2))/2-q1*log(q1)-q2*log(q2);

        Hvalues[t] = H;

        if (H < minimum) {
            determinedThreshold = t;
            minimum = H;
        }
    }

    *threshold = determinedThreshold;
    return E_SUCCESS;
}