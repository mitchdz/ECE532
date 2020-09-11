#include <stdio.h>
#include <stdlib.h>
#include "pngio.h"
#include "pngOpen.c"
#include "pngReadHdr.c"
#include "pngErrorHandler.c"

#define INPUT_PICTURE "horse.png"
#define THRESH 60;

void RodriguezEdgeDetector();

int main(int argc,char* argv[])
{
    if (argc==1) {
        RodriguezEdgeDetector();
    }
    if (argc>=2) {
        printf("please provide no inputs to the program.\n"); goto exit;
    }
exit:
    return 0;
}

// Rodriguez Edge Detector
// modified 3x3 gradient operator that computes an edge map
// E(r,c) from a grayscale input image I(r,c) as follows:
// TODO: finish description
void RodriguezEdgeDetector()
{
    int32_t nrows, ncols;
    PNGFILE *pngfile = pngOpen(INPUT_PICTURE, "r");
    pngReadHdr(pngfile, &nrows, &ncols);

    printf("%d rows, %d cols", nrows, ncols);

}





