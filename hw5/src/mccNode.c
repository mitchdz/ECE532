#include "mccNode.h"
#include <stdbool.h>
#include <stdlib.h>

void initializeLabelNode(labelNode *ln)
{
    ln->label = 0;
    ln->next = NULL;
}

void initializeSetNode(setNode *sn)
{
    sn->labels = (labelNode *)malloc(sizeof(labelNode));
    sn->labels->next = NULL;
    sn->labels->label = 0;
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

// be careful that setID exists
void addEquivalenceLabel(setNode* head, int setID, int label)
{
    // get the setID node
    setNode *tempSN = getSetNode(head, setID);
    if (tempSN == NULL) {
        printf("getSetNode returned NULL in addEquivalenceLabel\n");
        abort();
    }

    labelNode *labelHead = tempSN->labels;

    // iterate through label nodes
    while (labelHead != NULL && labelHead->next != NULL) {
        // only iterates to last 
        if (labelHead->label == label || labelHead->next->label == label) {                
            return;
        }
        labelHead = labelHead->next;
    }

    
    // create new label node
    labelNode *newLabelNode = (labelNode *)malloc(sizeof(labelNode));
    initializeLabelNode(newLabelNode);
    newLabelNode->label = label;

    // add new label node at end
    labelHead->next = newLabelNode;
}

void addSetID(setNode* head, int setID)
{
    while (head != NULL && head->next != NULL) {
        if (head->setID == setID || head->next->setID == setID) {
            return;
        }
        head = head->next;
    }
   
    setNode *tempSN = (setNode *)malloc(sizeof(setNode));
    initializeSetNode(tempSN);
    tempSN->setID = setID;

    head->next = tempSN;
}
