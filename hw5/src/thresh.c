#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <float.h>
#include <math.h>
#include "thresh.h"

#define ARRAY_SIZE 256

void convert2DPseudoArrayToHistogram(uint8_t **grayscale, int32_t n_rows,
        int32_t n_cols, uint8_t *Histogram)
{
    int r, c;
    for (r=0;r<n_rows;r++) {
        for (c=0;c<n_cols;c++) {
            Histogram[grayscale[r][c]]++;
        }
    }
    return;
}

void RecursiveUpdateFormula(uint8_t *h, int *threshold, double *Hvalues)
{
    double q1prev=0, var1prev=0, mu1prev=0, mu1=0, mu2=0, q1=0, q2=0,
           q2prev=0, var2prev=0, mu2prev=0, var1=0, var2=0, H=0, mu=0;
    int i = 0, first = -1, last = -1, t = 0;
    double numGrays=256.0, sum=0;

    double q1vals   [ARRAY_SIZE] = {0}, 
           q2vals   [ARRAY_SIZE] = {0}, 
           mu1vals  [ARRAY_SIZE] = {0},
           mu2vals  [ARRAY_SIZE] = {0}, 
           var1vals [ARRAY_SIZE] = {0}, 
           var2vals [ARRAY_SIZE] = {0},
           P        [ARRAY_SIZE] = {0};

    double determinedThreshold = DBL_MAX;
    double minimum = DBL_MAX;

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
        return;

    //explicitly zero out Hvalues if not done before
    for (t = 0; t < 256;t++) Hvalues[t] = 0;

    //calculate P vals
    for (t = first; t < last; t++) {

        P[t] = (h[t]/numGrays)/100.0;
    }

    //calculate q1 vals // note: q1[0] == 0
    q1vals[first] = P[first];
    for (t = first+1; t < last; t++) {
        q1vals[t] = q1vals[t-1] + P[t];
    }

    //calculate q2 vals // note: q2[0] == 0
    for (t = first; t < last; t++) {
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
    mu2vals[first] = mu/q2vals[first];
    for (t = first+1; t < last; t++) {
        q1 = q1vals[t];
        q2 = q2vals[t];
        mu1 = mu1vals[t];

        mu2vals[t] = (mu-q1*mu1)/q2; 
    }

    //calculate var1 vals
    for (t = first+1; t < last; t++) {
        q1prev   = q1vals[t-1];
        var1prev = var1vals[t-1];
        mu1prev  = mu1vals[t-1];
        mu1      = mu1vals[t];
        q1       = q1vals[t];

        var1vals[t] = (q1prev*(var1prev+(mu1prev-mu1)*(mu1prev-mu1)) +P[t]*(t-mu1)*(t-mu1))/q1; 
    }

    //calculate var2 vals
    sum = 0;
    for (i = 1; i <= 255; i++) {
        sum += pow(i-mu1vals[first],2)*(P[i]/q2vals[first]);
    }
    var2vals[first] = sum;
    for (t = first+1; t < last; t++) {
        q2prev   = q2vals[t-1];
        var2prev = var2vals[t-1];
        mu2prev  = mu2vals[t-1];
        mu2      = mu2vals[t];
        q2       = q2vals[t];

        var2vals[t] = (q2prev*(var2prev+(mu2prev-mu2)*(mu2prev-mu2)) -P[t]*(t-mu2)*(t-mu2))/q2; 
    }

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
    return;
}

void zeroPsuedo2DArray(void** array, int32_t n_rows, int32_t n_cols,
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
    return;
}


void threshold2DPseudoArray(uint8_t** array, int32_t n_rows, int32_t n_cols,
    int32_t t)
{
    int32_t r, c;
    for (r = 0; r < n_rows; r++) {
        for (c = 0; c < n_cols; c++) {
            array[r][c] = (array[r][c] >= t) ? 255 : 0;
        }
    }
    return;
}