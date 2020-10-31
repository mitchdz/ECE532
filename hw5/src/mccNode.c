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
    sn->numElements = 0;
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

void incrementNumSetID(setNode *head, int id)
{   
    head = head->next;
    while (head != NULL) {
        if (head->setID == id) {                
            head->numElements++;
            return;
        }
        head = head->next;
    }
    return;
}


// be careful that setID exists
void addEquivalenceLabel(setNode* head, int setID, int label)
{
    // get the setID node
    setNode *tempSN = getSetNode(head, setID);

    labelNode *labelHead = tempSN->labels;

    // iterate through label nodes
    while (labelHead != NULL && labelHead->next != NULL) {
        if (labelHead->label == label) {                
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
    // no checking if setID already exists
    while (head != NULL && head->next != NULL)
        head = head->next;        
    
    setNode *tempSN = (setNode *)malloc(sizeof(setNode));
    initializeSetNode(tempSN);
    tempSN->setID = setID;

    head->next = tempSN;
}

int getUniqueSetID(setNode* head)
{
    head=head->next;
    int i = 0;
    while (head != NULL) {
        i = head->setID; // always incrementing in value
        head = head->next;
    }
    return i+1;
}