/* mcc (maximally connected components) */

#include <stdbool.h>
#include <stdlib.h>
#include "mcc.h"
#include "mccNode.h"
#include "thresh.h"



int getLowestEquivalentLabel(uint8_t **ccM, int r, int c, setNode *head)
{
    int lowestLabel = INT_MAX;
    int currSet = ccM[r][c];
    
    setNode *sn = getSetNode(head, currSet);

    // sn->labels is the head node with label initialized to zero
    // every set at this point should have at least one labelNode
    if (sn->labels->next == NULL) {
        printf("function getLowestEquivalentLabel could not accest sn->labels->next");
        abort();
    }

    labelNode *ln = sn->labels->next;

    while (ln != NULL ) {
        if (ln->label < lowestLabel) {
            lowestLabel = ln->label;
        }
        ln = ln->next;
    }

    return lowestLabel;
}


void findMaximal8ConnectedForegroundComponents(IMAGE *img, uint8_t **ccM)
{
    int r,c, i;
    bool foreground;
    int uniqueLabel, smallestLabel=0;
    int32_t tempLabel = INT32_MAX;

    setNode *head = (setNode *)malloc(sizeof(setNode));
    initializeSetNode(head);

    //explicitly zero outputMatrix if not done before
    for(r=0;r<img->n_rows;r++){
        for(c=0;c<img->n_cols;c++){
            ccM[r][c] = 0;
        }
    }

    int nw, n, ne, w, e, sw, s, se;

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
            foreground = ( img->raw_bits[r][c] > img->threshold) ? true : false;
            if (!foreground) continue;

            nw = ccM[r-1][c-1];   n  = ccM[r-1][ c ];   ne = ccM[r-1][c+1];
            w  = ccM[ r ][c-1];                         e  = ccM[ r ][c+1];
            sw = ccM[r+1][c-1];   s  = ccM[r+1][ c ];   se = ccM[r+1][c+1];

            // get 8 neighbor pixels
            n[0] = ( nw > 0) ? nw : 0;
            n[1] = ( n  > 0) ? n  : 0;
            n[2] = ( ne > 0) ? ne : 0;
            n[3] = ( w  > 0) ? w  : 0;
            n[4] = ( e  > 0) ? e  : 0;
            n[5] = ( sw > 0) ? sw : 0;
            n[6] = ( s  > 0) ? s  : 0;
            n[7] = ( se > 0) ? se : 0;

            // if no unique neighbors, label current element and continue
            if ((n[0]+n[1]+n[2]+n[3]+n[4]+n[5]+n[6]+n[7]) == 0) {
                uniqueLabel = getUniqueSetID(head);
                ccM[r][c] = uniqueLabel;
                addSetID(head, uniqueLabel);
                continue;
            }

            // else if there are neighbors, find smallest label and assign
            tempLabel = INT32_MAX;
            for (i = 0; i < 8; i++) {
                if (n[i]  > 0) { 
                    tempLabel = (n[i]  < tempLabel) ? n[i] : tempLabel; 
                }
            }
            smallestLabel = tempLabel;
            ccM[r][c] = smallestLabel;
            incrementNumSetID(head, smallestLabel);

            // store equivalence between neighboring labels
            // addEquivalenceLabel checks for duplicates, no need to check here
            for (i = 0; i < 8; i++) {
                tempLabel = n[i];
                if (tempLabel > 0) {
                    addEquivalenceLabel(head, smallestLabel, tempLabel);
                }
            }

        } // end cols
    } // end rows

    // second pass - replace each temp label by the smallest label of 
    //               equivalence class

    int lowestEquivalentLabel = INT_MAX;
    for (r = 1; r < img->n_rows-1; r++) { // raster scanning
        for (c = 1; c < img->n_cols-1; c++) {

            // only worry about foreground pixels
            foreground = ( img->raw_bits[r][c] > img->threshold) ? true : false;
            if (!foreground) continue;
            
            lowestEquivalentLabel = getLowestEquivalentLabel(ccM,r,c, head);

            ccM[r][c] = lowestEquivalentLabel;

        } // end col 2nd pass
    } // end row 2nd pass

}


typedef struct finalsetnode {
    int setID;
    uint8_t color;
    struct finalsetnode *next;
} finalSetNode;

uint8_t findColor(finalSetNode *fsnHead, int setID)
{
    uint8_t setColor = 0;
    //check if setID is in list
    while (fsnHead != NULL && fsnHead->next != NULL) {
        if (fsnHead->setID == setID) {
            return fsnHead->color;
        }
        // only goes to second-last element, gotta check fsnHead->next :)
         if (fsnHead->next->setID == setID) {
            return fsnHead->next->color;
        }       
        fsnHead = fsnHead->next;
    }

    // setID does not exist; add to set and create color
    finalSetNode *fsn = (finalSetNode *)malloc(sizeof(finalSetNode));
    fsn->setID = setID;
    fsn->color = fsnHead->color + 10;
    fsn->next = NULL;

    // only iterated to last node, not past, so we can assign fsnHead->next
    fsnHead->next = fsn;
    return fsn->color;
}

void OverlayComponentsOntoImage(IMAGE *img, uint8_t **ccM)
{
    int r,c, setID;
    bool foreground;

    finalSetNode *fsnhead = (finalSetNode *)malloc(sizeof(finalSetNode));
    fsnhead->setID = 127;
    fsnhead->next = NULL;

    for (r = 1; r < img->n_rows-1; r++) { // raster scanning
        for (c = 1; c < img->n_cols-1; c++) {

            // only worry about foreground pixels
            foreground = ( ccM[r][c] > 0) ? true : false;
            if (!foreground) continue;

            setID = ccM[r][c];
            img->raw_bits[r][c] = findColor(fsnhead, setID);

        } // end col 2nd pass
    } // end row 2nd pass

    return;
}
