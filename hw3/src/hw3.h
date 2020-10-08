#pragma once
#include <stdint.h>
#include "pngio.h"


#define INPUT_PICTURE "input/edges.png"
#define OUTPUT_HOUGH_PEAKS "out/HoughPeaks.png"
#define OUTPUT_HOUGH_PEAKS_CSV "out/HoughPeaks.dat"
#define OUTPUT_HOUGH_ARRAY "out/HoughArray.png"
#define OUTPUT_HOUGH_LINES "out/HoughLines.png"

enum _hw3_error
{
    E_SUCCESS = 0,
    E_NO_NODES = -1,
};

typedef enum _hw3_error error_t;

struct _errordesc{
    int code;
    char *message;
} errordesc[] = {
    { E_SUCCESS, "No error" },
    { E_NO_NODES, "No nodes in linked list" },
};

typedef struct HoughPeakNode {
    uint8_t intensity;
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

void printHoughPeak(double m, double b, double rho, double theta, uint8_t intensity);
void populateHoughPeaksList(uint8_t** HA, HoughPeakNode* head);
void writeHoughPeakcsv(FILE* f, double m, double b);
enum _hw3_error printHoughLineEquations(HoughPeakNode* head);
void printHoughPeak(double m, double b, double rho, double theta, uint8_t intensity);
enum _hw3_error printHoughPeaks(HoughPeakNode* head);
void printHoughLineEquation(double m, double b);





