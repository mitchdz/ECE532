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
#include <math.h>

int32_t N_COLS = 0;
int32_t N_ROWS = 0;

int main(int argc,char* argv[])
{
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
    int32_t c, r;

    PNGFILE *pngfile = pngOpen(INPUT_PICTURE, "r");
    pngReadHdr(pngfile, &N_ROWS, &N_COLS);
    printf("image is %d rows by %d cols\n", N_ROWS, N_COLS);

    uint8_t **png_raw =       matalloc(N_ROWS, N_COLS, 0, 0, sizeof(uint8_t));
    double **JJRGMagnitudes = matalloc(N_ROWS, N_COLS, 0, 0, sizeof(double));
    double **SGMagnitudes =   matalloc(N_ROWS, N_COLS, 0, 0, sizeof(double));
    uint8_t **JJREEDGE =      matalloc(N_ROWS, N_COLS, 0, 0, sizeof(uint8_t));
    uint8_t **SGEEDGE =       matalloc(N_ROWS, N_COLS, 0, 0, sizeof(uint8_t));

    //store entire image into memory
    for (r = 0; r < N_ROWS; r++) pngReadRow(pngfile, png_raw[r]);

    // store JJR and SG magnitudes of each pixel
    for (r = 1; r < N_ROWS - 1; r++) {
        for (c = 1; c < N_COLS - 1; c++) {
            JJRGMagnitudes[r][c] = JJRGradiantMagnitude(png_raw, r, c);
            SGMagnitudes[r][c] = SGradiantMagnitude(png_raw, r, c);
        }
    }

    // produce edge map
    for (r = 1; r < N_ROWS - 1; r++) {
        for (c = 1; c < N_COLS - 1; c++) {
            JJREEDGE[r][c] = (JJRGMagnitudes[r][c] >= THRESH) ? 255 : 0;
            SGEEDGE[r][c] = (SGMagnitudes[r][c] >= THRESH) ? 255 : 0;
        }
    }

    // write edge maps to file
    PNGFILE *JJR_out = pngOpen(JJR_EDGE, "w");
    PNGFILE *SG_out =  pngOpen(SG_EDGE, "w");
    pngWriteHdr(JJR_out, N_ROWS, N_COLS);
    pngWriteHdr(SG_out,  N_ROWS, N_COLS);
    for (r = 0; r < N_ROWS; r++) {
        pngWriteRow(JJR_out, JJREEDGE[r]);
        pngWriteRow(SG_out,  SGEEDGE[r]);
    }

    //store R1 magnitude and edge values
    double R1_JJR_magnitude[5][5] = {0}, R1_SG_magnitude[5][5] =  {0};
    for (r = R1_R_START; r <= R1_R_END; r++) {
        for (c = R1_C_START; c <= R1_C_END; c++) {
            R1_JJR_magnitude[r-R1_R_START][c-R1_C_START] = JJRGMagnitudes[r][c];
            R1_SG_magnitude[r-R1_R_START][c-R1_C_START] = SGMagnitudes[r][c];
        }
    }

    uint8_t R1_JJR_edge[5][5] = {0}, R1_SG_edge[5][5]  = {0};
    for (r = 0; r < 5; r++) {
        for (c = 0; c < 5; c++) {
            R1_JJR_edge[r][c] = (R1_JJR_magnitude[r][c] > THRESH) ? 255 : 0;
            R1_SG_edge [r][c] = (R1_SG_magnitude [r][c] > THRESH) ? 255 : 0;
        }
    }

    printf("R1 JJR magnitudes\n"); printDouble5x5(R1_JJR_magnitude); printf("\n");
    printf("R1 JJR edge map\n");  printuint8_t5x5(R1_JJR_edge); printf("\n");
    printf("R1 SG  magnitudes\n"); printDouble5x5(R1_SG_magnitude); printf("\n");
    printf("R1 SG  edge map\n");  printuint8_t5x5(R1_SG_edge); printf("\n");

    goto cleanup;
cleanup:
    matfree(png_raw);
    pngClose(pngfile);
    pngClose(JJR_out);
    pngClose(SG_out);
}

int cmpfunc (const void *a, const void * b)
{
    return ( *(uint8_t*)a - *(uint8_t*)b );
}

double JJRMedian(uint8_t **pngfile, int32_t r, int32_t c,
        enum MedianDirection dir)
{
    double medianVal = 0;
    uint8_t lmedian = 0, rmedian = 0;
    uint8_t lvalues[5] = {0}, rvalues[5] = {0};
    switch(dir){
        case X:
            goto XMedian;
            break;
        case Y:
            goto YMedian;
            break;
    }
YMedian:
    lvalues[0] = pngfile[r][c-1];
    lvalues[1] = pngfile[r][c+1];
    lvalues[2] = pngfile[r-1][c-1];
    lvalues[3] = pngfile[r-1][c];
    lvalues[4] = pngfile[r-1][c+1];

    rvalues[0] = pngfile[r][c-1];
    rvalues[1] = pngfile[r][c+1];
    rvalues[2] = pngfile[r+1][c-1];
    rvalues[3] = pngfile[r+1][c];
    rvalues[4] = pngfile[r+1][c+1];

    qsort(lvalues, 5, sizeof(uint8_t), cmpfunc);
    qsort(rvalues, 5, sizeof(uint8_t), cmpfunc);

    lmedian = lvalues[2];
    rmedian = rvalues[2];
    medianVal = lmedian - rmedian;
    goto cleanup;
XMedian:
    lvalues[0] = pngfile[r-1][c];
    lvalues[1] = pngfile[r-1][c+1];
    lvalues[2] = pngfile[r][c+1];
    lvalues[3] = pngfile[r+1][c];
    lvalues[4] = pngfile[r+1][c+1];

    rvalues[0] = pngfile[r-1][c-1];
    rvalues[1] = pngfile[r-1][c];
    rvalues[2] = pngfile[r][c-1];
    rvalues[3] = pngfile[r+1][c-1];
    rvalues[4] = pngfile[r+1][c];

    qsort(lvalues, 5, sizeof(uint8_t), cmpfunc);
    qsort(rvalues, 5, sizeof(uint8_t), cmpfunc);

    lmedian = lvalues[2];
    rmedian = rvalues[2];
    medianVal = lmedian - rmedian;
    goto cleanup;

cleanup:
    return medianVal;
}

// Rodriguez Edge Detector
// modified 3x3 gradient operator
double JJRGradiantMagnitude(uint8_t **pngfile, int32_t row, int32_t col)
{
    double Iy, Ix, G;
    Iy = JJRMedian(pngfile, row, col, Y);
    Ix = JJRMedian(pngfile, row, col, X);

    G = sqrt(pow(Iy,2)+pow(Ix,2)) / 2;

    return G;
}

double Double3x3DotProduct(double a[3][3], int16_t b[3][3])
{
    double G = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            G += (a[i][j] * b[i][j]);
        }
    }
    return G;
}

//Sobel Gradient
double SGradiantMagnitude(uint8_t **pf, int32_t r, int32_t c)
{
    double Gx = 0, Gy = 0, G = 0;

    double x[3][3] = {
            {1, 0, -1},
            {2, 0, -2},
            {1, 0, -1}
    };

    double y[3][3] = {
            {-1, -2, -1},
            { 0,  0,  0},
            { 1,  2,  1}
    };

    int16_t A[3][3] = {
        {pf[r-1][c-1], pf[r-1][c], pf[r-1][c+1]},
        {pf[ r ][c-1], pf[ r ][c], pf[ r ][c+1]},
        {pf[r+1][c-1], pf[r+1][c], pf[r+1][c+1]}
    };

    Gx = Double3x3DotProduct(x,A);
    Gy = Double3x3DotProduct(y,A);

    G = sqrt( pow(Gx,2) + pow(Gy,2) ) / 8;

    return G;
}


void printDouble5x5(double a[5][5])
{
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            printf("%.2lf", a[i][j]);
            if (j != 4) printf(",");
        }
        printf("\n");
    }
    return;
}

void printuint8_t5x5(uint8_t a[5][5])
{
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            printf("%d", a[i][j]);
            if (j != 4) printf(",");
        }
        printf("\n");
    }
    return;
}

void printMap(double *map, int32_t nrows, int32_t ncols)
{
    //TODO: handle multiple digit numbers
        uint8_t i = 0, j = 0, k = 0;
        // printing ┌───┬───┬───┐
        printf("┌");
        for (i = 0; i < nrows -1; i++) {
                for (j = 0; j < nrows; j++) printf("─");
                printf("┬");
        }
        for (j = 0; j < nrows; j++) printf("─");
        printf("┐\n");

        //          ├───┼───┼───┤
        // printing │ 1 │ 2 │ 3 │
        //          ├───┼───┼───┤
        for (i = 0; i < ncols; i++) {
                double valueToPrint = *((map+i*nrows) + j);
                for (j = 0; j < nrows; j++) {
                        printf("│ %.2lf ", valueToPrint);
                }
                printf("│\n");

                if (i == nrows - 1) goto print_bottom;

                printf("├");
                for (k = 0; k < nrows - 1; k++) {
                        int extra_space = 0;
                        if ( valueToPrint > 10)  {extra_space = 3;}
                        if ( valueToPrint > 100) {extra_space = 4;}
                        for (j = 0; j < ncols + extra_space; j++) {
                            printf("─");
                        }
                        printf("┼");
                }
                for (j = 0; j < ncols; j++) printf("─");
                printf("┤\n");
        }
print_bottom:
        // printing └───┴───┴───┘
        printf("└");
        for (i = 0; i < nrows - 1; i++) {
                for (j = 0; j < nrows; j++) printf("─");
                printf("┴");
        }
        for (j = 0; j < nrows; j++) printf("─");
        printf("┘\n");
        return;
}
