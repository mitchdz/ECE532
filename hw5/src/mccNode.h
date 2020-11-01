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
void addEquivalenceLabel(setNode* head, int setID, int label);
void addSetID(setNode* head, int setID);
int getUniqueSetID(setNode* head);



#endif /* MCCNODE_H */