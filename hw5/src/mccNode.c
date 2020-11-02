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
    labelNode *tempLN = (labelNode *)malloc(sizeof(labelNode));
    initializeLabelNode(tempLN);
    tempLN->label = label;

    tempLN->next = (*head);
    (*head) = tempLN;
}

bool checkIfLabelExists(struct labelnode* head, int label)
{
    while (head != NULL) {
        if (head->label == label) return true;
        head = head->next;
    }
    return false;
}


// adds X labels into Y
void combineSetIDLabels(setNode* head, int setX, int labelY)
{
    labelNode *lnX = getSetNode(head,setX)->labels;
    labelNode *lnY = getSetNode(head,labelY)->labels;

    labelNode *tmp;

    bool exists;
    while (lnX != NULL) {
        tmp = lnY;
        exists = false; // assume false
        while (tmp != NULL && tmp->next != NULL) {
            if (tmp->label == lnX->label || tmp->next->label == lnX->label) {
                exists = true;
            }
            tmp=tmp->next;
        }
        if (!exists) { // if X label not found in Y, add the label to Y
            labelNode *tmpLabelNode = (labelNode *)malloc(sizeof(labelNode));
            initializeLabelNode(tmpLabelNode);
            tmpLabelNode->label=lnX->label;
            tmp->next=tmpLabelNode;
        }
        lnX = lnX->next;
    }
}

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
