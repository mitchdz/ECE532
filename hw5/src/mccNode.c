#include "mccNode.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

void initializeLabelNode(labelNode *ln)
{
    ln->label = 0;
    ln->next = NULL;
}

void initializeSetNode(setNode *sn)
{
    sn->labels = NULL;
    sn->next = NULL;
    sn->setID = 0;
}

setNode *getSetNode(setNode* head, int ID)
{
    while (head != NULL) {
        if (head->setID == ID) {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

bool findLabel(setNode* head, int setID, int label)
{
    // get the setID node
    setNode *tempSN = getSetNode(head, setID);
    if (tempSN == NULL) {
        printf("getSetNode returned NULL in addEquivalenceLabel\n");
        abort();
    }

    // iterate through label nodes
    labelNode *labelHead = tempSN->labels;
    while (labelHead != NULL) {
        if (labelHead->label == label) {                
            return true;
        }
        labelHead = labelHead->next;
    }

    return false;
}

void pushLabel(labelNode** head, int label)
{
    labelNode *tempLN = (setNode *)malloc(sizeof(labelNode));
    initializeLabelNode(tempLN);
    tempLN->label = label;

    tempLN->next = (*head);
    (*head) = tempLN;
}

// adds X labels into Y
void combineSetIDLabels(setNode* head, int setX, int labelY)
{
    labelNode *lnX = getSetNode(head,setX)->labels;
    labelNode *lnY = getSetNode(head,labelY)->labels;
    int xlabel;

    while (lnX != NULL) {
        xlabel = lnX->label;
        if ( !findLabel(head, labelY, xlabel) )
            pushLabel(&lnY, xlabel);
        lnX = lnX->next;
    }
}

// have to add union both setIDs
void unionEquivalenceLabel(setNode* head, int X, int Y)
{
    combineSetIDLabels(head, X, Y);
    combineSetIDLabels(head, Y, X);
}

void pushSetID(setNode** head, int ID)
{
    setNode *tempSN = (setNode *)malloc(sizeof(setNode));
    initializeSetNode(tempSN);
    tempSN->setID = ID;

    // necessary for unionEquivalenceLabel
    labelNode *ln = (labelNode *)malloc(sizeof(labelNode));
    initializeLabelNode(ln);
    ln->label = ID;
    tempSN->labels = ln;

    tempSN->next = (*head);
    (*head) = tempSN;
}
