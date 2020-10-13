#pragma once
#include <stdint.h>
#include <math.h>
#include "pngio.h"

#define INPUT_PICTURE "input/address.png"
#define INPUT_PICTURE_2 "input/graybook.png"
#define OUTPUT_THRESHOLD_FILENAME = "out/threshold.png"

/* hw4 error codes for debugging the program */
typedef enum _hw4_error
{
    E_SUCCESS = 0,
    E_NO_NODES = -1,
} _hw4_error;

/* helper function to easily decode error message */
struct _errordesc{
    int code;
    char *message;
} errordesc[] = {
    { E_SUCCESS,  (char *)"No error" },
    { E_NO_NODES, (char *)"No nodes in linked list" },
};

void analyzeImage();
