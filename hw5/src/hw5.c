#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "hw5.h"
#include "thresh.h"
#include "mcc.h"
#include "IO.h"
#include "hw5_test.h"

void printECE576AHW5Help()
{
    printf("USAGE\n");
    printf("the following flags can be enabled:\n");
    printf("\t-i <filename>\n");
    printf("\t\t<filename> is the name of the input file\n");
    printf("\t-o <filename>\n");
    printf("\t\t<filename> is the name of the output file\n");
    printf("\t-m <value>\n");
    printf("\t\t<value> MaxOutputValue should be either 0 or 255 where 0 ");
    printf("would indicate ncomponents\n");
    printf("\t-c <value>\n");
    printf("\t\t<value> is either 0 or 1 indicating ComponentGrayLevel\n");
    printf("\t-v\n");
    printf("\t\tverbose\n");
}

int main(int argc,char* argv[]) {
    error_ECE576A_t err = E_ECE576A_GENERIC_ERROR;
    int MaxOutputValue = 1;
    int ComponentGrayLevel = 1;
    bool verbose = false;
    char inputFile[100];
    char outputFile[100];

    char*ifile;
    int c;
    opterr = 0;
    while ((c = getopt (argc, argv, "ho:i:c:m:v")) != -1)
         switch (c)
         {
            case 'h':
              printECE576AHW5Help();
              return 1;
            case 'm':
              MaxOutputValue = *optarg;
              break;
            case 'o':
              strcpy(outputFile, (const char *)optarg);
              break;
            case 'i':
              ifile = optarg;
              strcpy(inputFile, ifile);
              break;
             case 'v':
              verbose = true;
              break;
            case 'c':
              ComponentGrayLevel = atoi(optarg);
              break;
            default:
              abort ();
         }

    if (inputFile == NULL)
        printError(err, "no input file specified");

    if (inputFile == NULL)
        printError(err, "no input file specified");

    err = ECE576A_HW5(inputFile, outputFile, MaxOutputValue, ComponentGrayLevel, verbose);
    if (err != E_ECE576A_SUCCESS) {
        printError(err, "error running hw5");
        return 1;
    }

    //test_ECE576A_HW5_2(true);
    //test_ECE576A_HW5_1(true);

    return 0;
}


error_ECE576A_t ECE576A_HW5(
    const char* inputImageFileName,
    const char* outputImageFileName,
    uint8_t MOV,
    uint8_t CGL,
    bool verbose)
{
    error_ECE576A_t err = E_ECE576A_SUCCESS;
    IMAGE IMG;

    // read the input PNG
    readPNGandClose((char *)inputImageFileName, &IMG);

    uint8_t histogram[256];
    convert2DPseudoArrayToHistogram(IMG.raw_bits, IMG.n_rows, IMG.n_cols, histogram);

    // find all 8-connected foreground components
    uint8_t **componentMatrix = matalloc(IMG.n_rows, IMG.n_cols, 0, 0, sizeof(uint8_t));
    int nc; //number of components
    //findMaximal8ConnectedForegroundComponents(&IMG, componentMatrix, CGL, &nc, verbose);
    iterativeCCL(&IMG, componentMatrix, CGL, &nc, verbose);
    printf("Number of Connected Components: %d\n", nc);
    // overlay components
    OverlayComponentsOntoImage(&IMG, componentMatrix, nc, CGL, MOV);

    // write output
    writePNG(IMG.raw_bits, (char *)outputImageFileName, IMG.n_rows, IMG.n_cols);

    matfree(IMG.raw_bits);
    matfree(componentMatrix);

    return err;
}

