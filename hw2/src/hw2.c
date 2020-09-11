#include <stdio.h>
#include <stdlib.h>
#include "pngio.h"

#define INPUT_PICTURE horse.png
#define THRESH 60;

void RodriguezEdgeDetector();

int main(int argc,char* argv[])
{
    if(argc==1)
        RodriguezEdgeDetector();
    if(argc>=2)
        printf("please provide no inputs to the program.\n"); goto exit;
exit:
    return 0;
}





// Rodriguez Edge Detector
// modified 3x3 gradient operator that computes an edge map
// E(r,c) from a grayscale input image I(r,c) as follows:
//

void RodriguezEdgeDetector()
{
    PNGFILE *image;








}





