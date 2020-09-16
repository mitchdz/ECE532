#pragma once
#include <stdint.h>
#include "pngio.h"

#define INPUT_PICTURE "horse.png"
#define JJR_EDGE "hw2_JJR_EDGE.png"
#define SG_EDGE "hw2_SG_EDGE.png"
#define THRESH 60

#define R1_R_START 205
#define R1_R_END   209
#define R1_C_START 182
#define R1_C_END   186

#define R2_R_START 347
#define R2_R_END   351
#define R2_C_START 350
#define R2_C_END   354

enum MedianDirection {
    X = 0,
    Y = 1
};

double JJRMedian(uint8_t **pngfile, int32_t row, int32_t col, enum MedianDirection dir);
double JJRGradiantMagnitude(uint8_t **pngfile, int32_t row, int32_t col);
double SGradiantMagnitude(uint8_t **pngfile, int32_t r, int32_t c);
void analyzeImage();
void produce_output_edge_map();
void printMap(double *map, int32_t nrows, int32_t ncols);
void printDouble5x5(double a[5][5]);
void printuint8_t5x5(uint8_t a[5][5]);
