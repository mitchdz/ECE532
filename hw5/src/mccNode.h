#ifndef MCCNODE_H
#define MCCNODE_H

typedef struct labelnode {
    int label;
    struct labelnode* next;
} labelNode;

// each setID has a list of labels
typedef struct setnode {
    int setID;
    labelNode* labels;
    struct setnode* next;
} setNode;



setNode *getSetNode(setNode* head, int ID);
int getUniqueSetID(setNode* head);
void initializeSetNode(setNode *sn);
void initializeLabelNode(labelNode *ln);
void unionEquivalenceLabels(setNode* head, int X, int Y);
void pushSetID(setNode** head, int setID);
int getUniqueSetID(setNode* head);
void listSetNodesLabels(setNode* head, int setNode);
void listEquivalencetable(setNode* head);


#endif /* MCCNODE_H */