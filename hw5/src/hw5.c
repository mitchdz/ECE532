#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ECE576A.h"
#include "hw5.h"
#include "thresh.h"
#include "mcc.h"
#include "IO.h"

void printECE576AHW5Help()
{
    printf("USAGE\n");
    printf("the following flags can be enabled:\n");
    printf("\t-i <filename>\n");
    printf("\t\t<filename> is the name of the file");
    printf("\t-o <filename>\n");
    printf("\t\t<filename> is the name of the file");
    printf("\t-m <value>\n");
    printf("\t\t<value> MaxOutputValue should be either 0 or 255 where 0 would \
        indicate ncomponents");
    printf("\t-c <value>\n");
    printf("\t\t<value> is either 0 or 1 indicating ComponentGrayLevel");
}

int main(int argc,char* argv[]) {
    error_ECE576A_t err = E_ECE576A_GENERIC_ERROR;
    int MaxOutputValue = 0;
    int ComponentGrayLevel = 1;
    char *inputFile = NULL;
    char *outputFile = NULL;

    int c;
    opterr = 0;
    while ((c = getopt (argc, argv, "hoicm:")) != -1)
         switch (c)
         {
            case 'h':
              printECE576AHW5Help();
              return 1;
            case 'm':
              MaxOutputValue = *optarg;
              break;
            case 'o':
              outputFile = optarg;
              break;
            case 'i':
              inputFile = optarg;
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

error_ECE576A_t ECE576A_HW5(
    const char* inputImageFileName,
    const char* outputImageFileName,
    uint8_t MaxOutputValue,
    uint8_t ComponentGrayLevel)
{
    error_ECE576A_t err = E_ECE576A_SUCCESS;
    IMAGE IMG = {0,0,0,NULL};

    // read the input PNG
    err = readPNGandClose((char *)inputImageFileName, IMG);
    if (err != E_ECE576A_SUCCESS)
        {printError(err, "readPNGandClose eror"); return err;}

    // find adaptive threshold
    err = findAdaptiveThreshold(IMG);
    if (err != E_ECE576A_SUCCESS)
        {printError(err, "findAdaptiveThreshold error"); return err;}

    uint8_t **componentMatrix = matalloc(IMG.n_rows, IMG.n_cols, 0, 0, sizeof(uint8_t));
    // find all 8-connected foreground components
    err = findMaximal8ConnectedForegroundComponents(IMG, componentMatrix);
    if (err != E_ECE576A_SUCCESS)
      {printError(err, "findMaximal8ConnectedForegroundComponents"); return err;}

    // overlay components
    err = OverlayComponentsOntoImage(IMG, componentMatrix);
    if (err != E_ECE576A_SUCCESS)
      {printError(err, "OverlayComponentsOntoImage"); return err;}

    // write output
    writePNG(IMG.raw_bits, (char *)outputImageFileName, IMG.n_rows, IMG.n_cols);

    matfree(IMG.raw_bits);

    return err;
}
