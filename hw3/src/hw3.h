#pragma once
#include <stdint.h>
#include <math.h>
#include "pngio.h"

#define INPUT_PICTURE "input/edges.png"
#define OUTPUT_HOUGH_PEAKS "out/HoughPeaks.png"
#define OUTPUT_HOUGH_PEAKS_CSV "out/HoughPeaks.dat"
#define OUTPUT_HOUGH_ARRAY "out/HoughArray.png"
#define OUTPUT_HOUGH_LINES "out/HoughLines.png"

/* hw3 error codes for debugging the program
 */
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
    { E_SUCCESS,  (char *)"No error" },
    { E_NO_NODES, (char *)"No nodes in linked list" },
};


// struct to store hough peak information
typedef struct HoughPeakNode {
    uint8_t intensity;
    double theta;
    double rho;
    double m;
    double b;

    struct HoughPeakNode* next;
} HoughPeakNode;

void analyzeImage();
void HTStraightLine(uint8_t **edge_map, int32_t n_rows, int32_t n_cols,
        int32_t **HA);
void writePNG(uint8_t** raw_data, char* filename, int n_rows, int n_cols);

void printHoughPeak(double m, double b, double rho, double theta, uint8_t intensity);
void populateHoughPeaksList(uint8_t** HA, HoughPeakNode* head, int32_t n_rows,
        int32_t n_cols);
void writeHoughPeakcsv(FILE* f, double m, double b);
enum _hw3_error printHoughLineEquations(HoughPeakNode* head);
void printHoughPeak(double m, double b, double rho, double theta, uint8_t intensity);
enum _hw3_error printHoughPeaks(HoughPeakNode* head);
void printHoughLineEquation(double m, double b);


/* @brief printHoughPeaks iterate through a singly linked list
 * and prints values about the hough peaks
 */
enum _hw3_error printHoughPeaks(HoughPeakNode* head)
{
    //make sure there are more nodes than head node
    if (head->next == NULL) return E_NO_NODES;

    while (head->next != NULL) {
        head = head->next;
        printHoughPeak(head->m, head->b, head->rho, head->theta, head->intensity);
        if (head->next == NULL) break;
    }
    return E_SUCCESS;
}

/* @brief printHoughLineEquations takes singly linked list and prints
 * each line equation
 * @param head head of singly linked list
 */
enum _hw3_error printHoughLineEquations(HoughPeakNode* head)
{
    //make sure there are more nodes than head node
    if (head->next == NULL) return E_NO_NODES;

    while (head->next != NULL) {
        head = head->next;
        printHoughLineEquation(head->m, head->b);
        if (head->next == NULL) break;
    }
    return E_SUCCESS;
}

/* @brief print information about hough peak to terminal
 * @param m slope
 * @param b y intercept
 * @param rho rho value
 * @param theta theta value
 * @intensity uint8_t value in Hough Array scaled from 0 to 255
 */
void printHoughPeak(double m, double b, double rho, double theta, uint8_t intensity)
{
    printf("rho:%.1lf\t theta:%.2lf\t intensity:%d, m:%.2lf\t  b:%lf\n", rho, theta, intensity, m, b);
}


/* @brief given m and b, print y=mx+b format
 * @param m slope
 * @param b slope
 */
void printHoughLineEquation(double m, double b)
{
    if (m == -1.0/0.0) //check for negative infinity
        printf("x=%lf\n",b);
    else if (round(m) == 0)
        printf("y=%lf\n",b);
    else
        printf("y=%lfx+%lf\n", m, b);
}


/* @brief writePNG writes a png to a file
 * @param raw_data uint8_t pseudo 2-D array created by matalloc
 * @param n_rows number of rows of the pseudo 2D array
 * @param n_cols number of columns of the pseudo 2D array
 */
void writePNG(uint8_t** raw_data, char* filename, int n_rows, int n_cols)
{
    PNGFILE *output_png = pngOpen(filename, (char *)"w");
    pngWriteHdr(output_png, n_rows, n_cols);
    for (int r = 0; r < n_rows; r++) pngWriteRow(output_png, raw_data[r]);
    free(output_png);
}


/* @brief frees HoughPeakNode linked list
 * @param head top of the linked list
 */
void freeHoughPeakNodeList(HoughPeakNode* head)
{
    HoughPeakNode* tmp;
    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

/* @brief appends newNode to the end of the singly linked list
 * @param head head of the linked list
 * @param newNode node to be appended
 */
void appendHoughPeakNode(HoughPeakNode* head, HoughPeakNode* newNode)
{
    HoughPeakNode* tmpNode = head;
    while (tmpNode->next != NULL) {
        tmpNode = tmpNode->next;
    }
    tmpNode->next = newNode;
}

/* @brief finds number of nodes in HoughPeakNode list
 * @param head head of the linked list
 */
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

//TODO: for now, hand-drawing lines. Want to automate this in the future
void drawLineFromSlopeAndIntercept(uint8_t **HAlines, double m, double b)
{
    //TODO
}

// Bresenham's line drawing algorithm not used right now.
void drawLine(uint8_t **HAlines, int x0, int y0, int x1, int y1)
{
    int dx, dy, p, x, y;
    dx=x1-x0; dy=y1-y0;
    x=x0; y=y0; p=2*dy-dx;

    while(x<x1)
    {
        if(p>=0)
        {
            HAlines[x][y] = 255;
            y=y+1;
            p=p+2*dy-2*dx;
        }
        else
        {
            HAlines[x][y] = 255;
            p=p+2*dy;
        }
        x=x+1;
    }
}
