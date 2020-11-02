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

// adds X labels into Y
void combineSetIDLabels(setNode* head, int setX, int setY)
{
    labelNode *lnX = getSetNode(head,setX)->labels;
    labelNode *yLabelNode = NULL;
    int xLabel = -1;

    bool exists;
    while (lnX != NULL) {
        yLabelNode = getSetNode(head, setY)->labels;
        xLabel = lnX->label;
        exists = false; // assume false
        while (yLabelNode != NULL && yLabelNode->next != NULL && !exists) {
            // because we are only iterating to the last node, we need to
            // check yLabelNode->next->label because if the last label is in there
            // then this would not detect it.
            if (yLabelNode->label == xLabel || yLabelNode->next->label == xLabel) {
                exists= true;
            }
            yLabelNode=yLabelNode->next;
        }
        if (!exists) { // if X label not found in Y, add the label to Y
            labelNode *tmpLabelNode = (labelNode *)malloc(sizeof(labelNode));
            initializeLabelNode(tmpLabelNode);
            tmpLabelNode->label = xLabel;
            tmpLabelNode->next = NULL;
            yLabelNode->next=tmpLabelNode;
        }
        lnX = lnX->next;
    }
}

void unionEquivalenceLabel(setNode* head, int X, int Y)
{
    combineSetIDLabels(head, Y, X);
    combineSetIDLabels(head, X, Y);
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

void listSetNodesLabels(setNode* head, int setNode)
{
    labelNode *lnhead = getSetNode(head, setNode)->labels;
    printf("SetNode %d: ", setNode);
    while (lnhead != NULL) {
        printf("%d", lnhead->label);
        if (lnhead->next != NULL) printf(", ");
        lnhead=lnhead->next;
    }
    printf("\n");
}


void listEquivalencetable(setNode* head)
{
    setNode *tmp = head;
    while (tmp != NULL) {
        listSetNodesLabels(head, tmp->setID);
        tmp = tmp->next;
    }
}