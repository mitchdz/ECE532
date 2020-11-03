//
// Created by mitch on 11/2/20.
//

#include "hw5_test.h"

#include <stdio.h>

void run_tests()
{
    bool test1 = test_ECE576A_HW5_1(false);
    if (!test1) printf("test 1 fail\n");
    else (printf("test 1 pass\n"));
    bool test2 = test_ECE576A_HW5_2(false);
    if (!test2) printf("test 2 fail\n");
    else (printf("test 2 pass\n"));
}

int runhw5(IMAGE IMG, bool CGL, bool MOV, bool verbose)
{
    uint8_t **componentMatrix = matalloc(IMG.n_rows, IMG.n_cols, 0, 0, sizeof(uint8_t));
    int nc; //number of components
    findMaximal8ConnectedForegroundComponents(&IMG, componentMatrix, CGL, &nc, verbose);
    printf("Number of Connected Components: %d\n", nc);

    matfree(componentMatrix);
    return nc;
}

void printImagePixels(IMAGE img)
{
    for (int r = 0; r < img.n_rows; r++) {
        for (int c = 0; c < img.n_cols; c++) {
            printf("%d ", img.raw_bits[r][c]);
        }
        printf("\n");
    }
}

bool test_ECE576A_HW5_1(bool verbose)
{
    int CGL = 0, MOV = 0;

    IMAGE IMG;
    IMG.n_cols = 5;
    IMG.n_rows = 5;

    /* 0 0 0 0 0
     * 0 0 1 0 0
     * 0 1 1 1 0
     * 0 0 0 0 0
     * 0 0 0 0 0
     */
    uint8_t **tmpMatrix = matalloc(IMG.n_rows, IMG.n_cols, 0, 0, sizeof(uint8_t));
    for (int r = 0; r < IMG.n_rows; r++) {
        for (int c = 0; c < IMG.n_rows; c++) {
            tmpMatrix[r][c] = 1;
        }
    }
    tmpMatrix[1][2] = 0;
    tmpMatrix[2][1] = 0;
    tmpMatrix[2][2] = 0;
    tmpMatrix[2][3] = 0;

    IMG.raw_bits = tmpMatrix;
    printf("Test 1: CGL = 0 MOV = 0\n");
    if (verbose) printImagePixels(IMG);

    // act
    int nc = runhw5(IMG, CGL, MOV, verbose);

    // assert
    matfree(IMG.raw_bits);
    matfree(tmpMatrix);

    if (nc == 1) return true;
    return false;
}



// CGL = 1, MOV = 0, single component should be returned
bool test_ECE576A_HW5_2(bool verbose)
{
    //arrange
    int CGL = 1, MOV = 0;
    IMAGE IMG;
    IMG.n_cols = 6;
    IMG.n_rows = 6;

    uint8_t **tmpMatrix = matalloc(IMG.n_rows, IMG.n_cols, 0, 0, sizeof(uint8_t));
    for (int r = 0; r < IMG.n_rows; r++)
        for (int c = 0; c < IMG.n_cols; c++)
            tmpMatrix[r][c] = 0;
    /* 0 0 0 0 0 0 0
     * 0 0 1 0 1 0 0
     * 0 1 1 1 1 0 0
     * 0 0 0 0 0 0 0
     * 0 0 0 0 0 0 0 */
    tmpMatrix[1][2] = 1;
    tmpMatrix[1][4] = 1;
    tmpMatrix[2][1] = 1;
    tmpMatrix[2][2] = 1;
    tmpMatrix[2][3] = 1;
    tmpMatrix[2][4] = 1;

    IMG.raw_bits = tmpMatrix;

    printf("Test 1: CGL = 1 MOV = 0\n");
    if (verbose) printImagePixels(IMG);

    // act
    int nc = runhw5(IMG, CGL, MOV, verbose);

    // assert
    matfree(IMG.raw_bits);
    matfree(tmpMatrix);
    if (nc == 1) return true;
    return false;
}
