/* mcc (maximally connected components) */

#include <stdbool.h>
#include <stdlib.h>
#include "mcc.h"
#include "mccNode.h"
#include "thresh.h"
#include <limits.h>

static bool checkForeground(int value, bool CGL)
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

static int getLowestEquivalentLabel(setNode *head, int setID)
{
    labelNode *ln = getSetNode(head, setID)->labels;

    int lowestLabel = INT_MAX;
    while (ln != NULL ) {
        if (lowestLabel > ln->label)
            lowestLabel = ln->label;

        ln = ln->next;
    }

    return lowestLabel;
}

void iterativeCCL(IMAGE *img, uint8_t **outccM, bool CGL, int *nc, bool verbose)
{
    int r,c,i,M;
    int n[8], NW, N, NE, W, E, SW, S, SE; // neighbors
    int **l = matalloc(img->n_rows, img->n_cols, 0, 0, sizeof(int));
    for(r=0;r<img->n_rows;r++){
        for(c=0;c<img->n_cols;c++){
            l[r][c] = 0;
        }
    }

    int nextLabel = 1;
    for (r=1;r<img->n_rows-1;r++) { // raster scanning
        for (c=1;c<img->n_cols-1;c++) {
            // only worry about foreground pixels
            checkForeground(img->raw_bits[r][c], CGL) ? (l[r][c] = nextLabel++) : (l[r][c] = 0);
        }
    }
    bool change=false;
    do
    {
        change = false;
        for (r =1; r < img->n_rows-1;r++)
            for (c = 1; c < img->n_cols - 1; c++)
                if (l[r][c] != 0) {
                    NW = l[r-1][c-1];   N  = l[r-1][ c ];   NE = l[r-1][c+1];
                    W  = l[ r ][c-1];                       E  = l[ r ][c+1];
                    SW = l[r+1][c-1];   S  = l[r+1][ c ];   SE = l[r+1][c+1];

                    // get 8 neighbor pixels (8-connectivity)
                    n[0] = (NW > 0) ? NW : 0; n[1] = (N > 0) ? N : 0; n[2] = (NE > 0) ? NE : 0;
                    n[3] = (W > 0 ) ? W  : 0;                         n[4] = (E > 0)  ? E : 0;
                    n[5] = (SW > 0) ? SW : 0; n[6] = (S > 0) ? S : 0; n[7] = (SE > 0) ? SE : 0;

                    M = INT_MAX;
                    for (i = 0; i < 8; i++) {
                        if (M > n[i] && n[i] != 0)
                            M = n[i];
                    }

                    if (M != l[r][c]) {
                        l[r][c] = M;
                        change = true;
                    }
                }
        for (r = img->n_rows-1; r > 0; r--)
            for (c = img->n_cols-1; c > 0; c--)
                if (l[r][c] != 0) {
                    NW = l[r-1][c-1];   N  = l[r-1][ c ];   NE = l[r-1][c+1];
                    W  = l[ r ][c-1];                       E  = l[ r ][c+1];
                    SW = l[r+1][c-1];   S  = l[r+1][ c ];   SE = l[r+1][c+1];

                    // get 8 neighbor pixels (8-connectivity)
                    n[0] = (NW > 0) ? NW : 0; n[1] = (N > 0) ? N : 0; n[2] = (NE > 0) ? NE : 0;
                    n[3] = (W > 0 ) ? W  : 0;                         n[4] = (E > 0)  ? E : 0;
                    n[5] = (SW > 0) ? SW : 0; n[6] = (S > 0) ? S : 0; n[7] = (SE > 0) ? SE : 0;

                    M = INT_MAX;
                    for (i = 0; i < 8; i++) {
                        if (M > n[i] && n[i] != 0)
                            M = n[i];
                    }

                    if (M != l[r][c]) {
                        l[r][c] = M;
                        change = true;
                    }
                }
    } // end do
    while (change == true);

    // copy each value of l into ccM, and count number of components
    int *setCounts = (int *)malloc(sizeof(int)*nextLabel);
    for (i = 0; i < nextLabel;i++) setCounts[i] = 0;
    // have to copy each value of l into outccM
    for (r = 1; r < img->n_rows-1; r++) { // raster scanning
        for (c = 1; c < img->n_cols-1; c++) {
            outccM[r][c] = l[r][c];
            setCounts[l[r][c]]++;
        } // end col 2nd pass
    } // end row 2nd pass

    int numSets = 0;
    for (i = 0; i < nextLabel;i++) {
        if (setCounts[i] > 0) numSets++;
        //if (verbose) printf("set %d:%d\n",i, setCounts[i]);
    }
    free(setCounts);

    *nc = numSets -1 ;
}


// this function does not work properly yet, it is super close. But no cigar iterativeCCL works.
void findMaximal8ConnectedForegroundComponents(IMAGE *img, uint8_t **outccM, 
    bool CGL, int *nc, bool verbose)

{
    int r,c, i, smallestSetID, uniqueLabel = 1;

    setNode *equivalenceTable = NULL;

    int **ccM = matalloc(img->n_rows, img->n_cols, 0, 0, sizeof(int));
    for(r=0;r<img->n_rows;r++){
        for(c=0;c<img->n_cols;c++){
            ccM[r][c] = 0;
        }
    }

    int NW, N, NE, W;

    int n[4]; //neighbor
    /*  0 | 1 | 2
     *  -----------
     *  3 | * |
     *  -----------
     *    |   |
     */
    // Hoshen-Kopelman algorithm
    // first pass - assign temporary labels and record equivalences
    for (r = 1; r < img->n_rows-1; r++) { // raster scanning
        for (c = 1; c < img->n_cols-1; c++) {

            // only worry about foreground pixels
            if (!checkForeground(img->raw_bits[r][c], CGL)) continue;

            NW = ccM[r-1][c-1];   N  = ccM[r-1][ c ];   NE = ccM[r-1][c+1];
            W  = ccM[ r ][c-1];

            // get 4 neighbor pixels (8-connectivity)
            n[0] = ( NW > 0) ? NW : 0;
            n[1] = ( N  > 0) ? N  : 0;
            n[2] = ( NE > 0) ? NE : 0;
            n[3] = ( W  > 0) ? W  : 0;

            // if no unique neighbors, l current element and continue
            if ((n[0] == 0) && (n[1] == 0) && (n[2] == 0) && (n[3] == 0)) {
                ccM[r][c] = uniqueLabel;
                pushSetID(&equivalenceTable, uniqueLabel);
                uniqueLabel++;
                continue;
            }

            // else if there are neighbors, find smallest l and assign
            smallestSetID = INT_MAX;
            for (i = 0; i < 4; i++) {
                if (smallestSetID > n[i] && n[i] != 0)
                    smallestSetID = n[i];
            }
            ccM[r][c] = smallestSetID;

            // store equivalence between neighboring labels
            for (i = 0; i < 4; i++) {
                if (n[i] != 0)
                    unionEquivalenceLabels(equivalenceTable, smallestSetID, n[i]);
            }

        } // end cols
    } // end rows

    // second pass - replace each temp l by the smallest l of
    //               equivalence class

    int *setCounts = (int *)malloc(sizeof(int)*uniqueLabel);
    for (i = 0; i < uniqueLabel;i++) setCounts[i] = 0;

    int lowestEquivalentLabel = INT_MAX;
    for (r = 1; r < img->n_rows-1; r++) {
        for (c = 1; c < img->n_cols-1; c++) {

             // only worry about foreground pixels
            if (!checkForeground(img->raw_bits[r][c], CGL)) continue;
           
            lowestEquivalentLabel = getLowestEquivalentLabel(equivalenceTable, ccM[r][c]);

            ccM[r][c] = lowestEquivalentLabel;

            setCounts[lowestEquivalentLabel]++;
        } // end col 2nd pass
    } // end row 2nd pass

    int numSets = 0;
    for (i = 0; i < uniqueLabel;i++) {
        if (setCounts[i] > 0) numSets++;
        if (verbose) printf("set %d:%d\n",i, setCounts[i]);
    }
    free(setCounts);

    if (verbose) listEquivalencetable(equivalenceTable);

    int setVal;
    // have to copy each value of l into outccM
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
            if (CGL) { outputPixelValue = round(1.0*k*maxOutputValue/nc);    }
            else     { outputPixelValue = round(1.0*(k-1)*maxOutputValue/nc);}

            img->raw_bits[r][c] = outputPixelValue;

        } // end col 2nd pass
    } // end row 2nd pass
    return;
}
