#pragma once
#include <stdint.h>
#include "pngio.h"

#define HOUGH_ROWS 100
#define HOUGH_COLS 100

#define INPUT_PICTURE "input/edges.png"
#define OUTPUT_PICTURE "out/HoughPeaks.png"
#define OUTPUT_HOUGH_ARRAY "out/HoughArray.png"
#define OUTPUT_HOUGH_LINES "out/HoughLines.png"

typedef struct HoughPeakNode {
    double theta;
    double rho;
    double m;
    double b;

    struct HoughPeakNode* next;
} HoughPeakNode;

void freeHoughPeakNodeList(HoughPeakNode* head)
{
    HoughPeakNode* tmp;
    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp);
    }

}

void appendHoughPeakNode(HoughPeakNode* head, HoughPeakNode* new)
{
    HoughPeakNode* tmpNode = head;
    while (tmpNode->next != NULL) {
        tmpNode = tmpNode->next;
    }
    tmpNode->next = new;
}

int  getNumberHoughPeakNodes(HoughPeakNode* head)
{
    int count = -1;
    HoughPeakNode* tmpNode = head;
    while (tmpNode != NULL) {
        tmpNode = tmpNode->next;
        count++;
    }
    return count;
}

void analyzeImage();
void HTStraightLine(uint8_t **edge_map, int32_t n_rows, int32_t n_cols,
        int32_t **HA);
void writePNG(uint8_t** raw_data, char* filename, int n_rows, int n_cols);
