#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "hw5.h"
#include "thresh.h"
#include "mcc.h"
#include "IO.h"
#include "ECE576A.h"

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
}

int main(int argc,char* argv[]) {
    error_ECE576A_t err = E_ECE576A_GENERIC_ERROR;
    int MaxOutputValue = 0;
    int ComponentGrayLevel = 1;
    char inputFile[100];
    char outputFile[100];

    char*ifile;
    int c;
    opterr = 0;
    while ((c = getopt (argc, argv, "ho:i:cm")) != -1)
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

    err = ECE576A_HW5(inputFile, outputFile, MaxOutputValue,
        ComponentGrayLevel);
    if (err != E_ECE576A_SUCCESS) {
        printError(err, "error running hw5");
        return 1;
    }

    return 0;
}

//Otsu's method for finding adaptive threshold
void findOtsuAdaptiveThreshold(IMAGE *img)
{
    img->threshold = 127;
    return;
}

error_ECE576A_t ECE576A_HW5(
    const char* inputImageFileName,
    const char* outputImageFileName,
    uint8_t MaxOutputValue,
    uint8_t ComponentGrayLevel)
{
    error_ECE576A_t err = E_ECE576A_SUCCESS;
    IMAGE IMG;

    // read the input PNG
    readPNGandClose((char *)inputImageFileName, &IMG);

    // find adaptive threshold
    findOtsuAdaptiveThreshold(&IMG);

    // find all 8-connected foreground components
    uint8_t **componentMatrix = matalloc(IMG.n_rows, IMG.n_cols, 0, 0, sizeof(uint8_t));
    findMaximal8ConnectedForegroundComponents(&IMG, componentMatrix);

    // overlay components
    OverlayComponentsOntoImage(&IMG, componentMatrix);

    // write output
    writePNG(IMG.raw_bits, (char *)outputImageFileName, IMG.n_rows, IMG.n_cols);

    matfree(IMG.raw_bits);
    matfree(componentMatrix);

    return err;
}
