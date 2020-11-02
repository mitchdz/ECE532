/* mcc (maximally connected components) */

#include <stdbool.h>
#include <stdlib.h>
#include "mcc.h"
#include "mccNode.h"
#include "thresh.h"
#include <limits.h>

bool checkForeground(int value, bool CGL); //private for this file

bool checkForeground(int value, bool CGL)
{
    bool foreground;
    if (CGL) { //ComponentGrayLevel is 1 meaning black=foreground
        foreground = ( value > 0) ? true : false;
    }
    else {
        foreground = ( value < 255) ? true : false;
    }
    return foreground;
}

int getLowestEquivalentLabel(int **ccM, int r, int c, setNode *head)
{
    int currSet = ccM[r][c];
    labelNode *ln = getSetNode(head, currSet)->labels;

    int lowestLabel = INT_MAX;
    while (ln != NULL ) {
        if (lowestLabel > ln->label)
            lowestLabel = ln->label;

        ln = ln->next;
    }

    return lowestLabel;
}


void findMaximal8ConnectedForegroundComponents(IMAGE *img, uint8_t **outccM, 
    bool CGL, int *nc)
{
    int r,c, i, tmp, smallestLabel, uniqueLabel = 0;

    setNode *head = NULL;

    int **ccM = matalloc(img->n_rows, img->n_cols, 0, 0, sizeof(int));
    for(r=0;r<img->n_rows;r++){
        for(c=0;c<img->n_cols;c++){
            ccM[r][c] = 0;
        }
    }

    uint8_t NW, N, NE, W, E, SW, S, SE;

    int n[8]; //neighbor
    /*  0 | 1 | 2
     *  -----------
     *  3 | * | 4
     *  -----------
     *  5 | 6 | 7
     */
    // Hoshen-Kopelman algorithm
    // first pass - assign temporary labels and record equivalences
    for (r = 1; r < img->n_rows-1; r++) { // raster scanning
        for (c = 1; c < img->n_cols-1; c++) {

            // only worry about foreground pixels
            if (!checkForeground(img->raw_bits[r][c], CGL)) continue;

            NW = ccM[r-1][c-1];   N  = ccM[r-1][ c ];   NE = ccM[r-1][c+1];
            W  = ccM[ r ][c-1];                         E  = ccM[ r ][c+1];
            SW = ccM[r+1][c-1];   S  = ccM[r+1][ c ];   SE = ccM[r+1][c+1];

            // get 8 neighbor pixels
            n[0] = ( NW > 0) ? NW : 0;
            n[1] = ( N  > 0) ? N  : 0;
            n[2] = ( NE > 0) ? NE : 0;
            n[3] = ( W  > 0) ? W  : 0;
            n[4] = ( E  > 0) ? E  : 0;
            n[5] = ( SW > 0) ? SW : 0;
            n[6] = ( S  > 0) ? S  : 0;
            n[7] = ( SE > 0) ? SE : 0;

            // if no unique neighbors, label current element and continue
            if ((n[0] == 0) && (n[1] == 0) && (n[2] == 0) && (n[3] == 0) &&
                (n[4] == 0) && (n[5] == 0) && (n[6] == 0) && (n[7] == 0)
            ) {
                ccM[r][c] = ++uniqueLabel;
                pushSetID(&head, uniqueLabel);
                continue;
            }

            // else if there are neighbors, find smallest label and assign
            smallestLabel = INT_MAX;
            for (i = 0; i < 8; i++) {
                tmp = n[i];
                if (tmp == 0) continue;
                if (smallestLabel > tmp)
                    smallestLabel = tmp;
            }
            ccM[r][c] = smallestLabel;
            

            // store equivalence between neighboring labels
            for (i = 0; i < 8; i++) {
                tmp = n[i];
                if (tmp == 0) continue; // n[i] is setID, 0 is background
                unionEquivalenceLabel(head, ccM[r][c], tmp);
            }

        } // end cols
    } // end rows

    // second pass - replace each temp label by the smallest label of 
    //               equivalence class

    int setCounts[100000] = {0};


    int lowestEquivalentLabel = INT_MAX;
    for (r = 1; r < img->n_rows-1; r++) {
        for (c = 1; c < img->n_cols-1; c++) {

             // only worry about foreground pixels
            if (!checkForeground(img->raw_bits[r][c], CGL)) continue;
           
            lowestEquivalentLabel = getLowestEquivalentLabel(ccM,r,c,head);

            ccM[r][c] = lowestEquivalentLabel;

            setCounts[lowestEquivalentLabel]++;
        } // end col 2nd pass
    } // end row 2nd pass

    int numSets = 0;
    for (i = 0; i < 100;i++) {
        if (setCounts[i] > 0) numSets++;
        printf("set %d:%d\n",i, setCounts[i]);
    }


    int setVal;
    // have to copy each value of ccM into outccM
    for (r = 1; r < img->n_rows-1; r++) { // raster scanning
        for (c = 1; c < img->n_cols-1; c++) {               
            setVal = ccM[r][c];
            outccM[r][c] = setVal;
        } // end col 2nd pass
    } // end row 2nd pass

    *nc = numSets;
    return;
}

void OverlayComponentsOntoImage(IMAGE *img, uint8_t **ccM, int nc, bool CGL, 
    bool MOV)
{
    int r,c, k, outputPixelValue, maxOutputValue;

    if (MOV) { maxOutputValue = 255; }
    else     { maxOutputValue = nc; }

    for (r = 1; r < img->n_rows-1; r++) { // raster scanning
        for (c = 1; c < img->n_cols-1; c++) {

             // only worry about foreground pixels
            if (!checkForeground(img->raw_bits[r][c], CGL)) continue;
 
            k = ccM[r][c];
            if (CGL) { outputPixelValue = round(k*maxOutputValue/nc);    }
            else     { outputPixelValue = round((k-1)*maxOutputValue/nc);}

            img->raw_bits[r][c] = outputPixelValue;

        } // end col 2nd pass
    } // end row 2nd pass
    return;
}
