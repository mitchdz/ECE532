#ifndef HW5_H
#define HW5_H

#include "ECE576A.h"
#include <stdbool.h>

void printECE576AHW5Help();
/* hw5 error codes for debugging the program */
enum _ECE576A_error
{
    E_ECE576A_SUCCESS,
    E_ECE576A_FILE_NOT_FOUND,
    E_ECE576A_GENERIC_ERROR,
    E_ECE576A_ARRAY_SIZE_MISMATCH,
    E_ECE576A_NOT_IMPLEMENTED,
    E_ECE576A_RECURSIVE_FIRST_LAST_NOT_FOUND
};

typedef enum _ECE576A_error error_ECE576A_t;

/* helper function to easily decode error message */
struct _errordesc{
    int code;
    char *message;
} errordesc[] = {
    { E_ECE576A_SUCCESS,  (char *)"No error" },
    { E_ECE576A_FILE_NOT_FOUND, (char *)"File not found" },
    { E_ECE576A_GENERIC_ERROR,  (char *)"Generic Error" },
    { E_ECE576A_ARRAY_SIZE_MISMATCH,  (char *)"Array sizes do not match" },
    { E_ECE576A_NOT_IMPLEMENTED,  (char *)"Not implemented yet" },
    { E_ECE576A_RECURSIVE_FIRST_LAST_NOT_FOUND, (char *)"Recursive first and last index not able to be found"},
    { E_ECE576A_NOT_IMPLEMENTED, (char *)"Funcionality not implemented"},
};

void printError(error_ECE576A_t E, char *msg)
{
    printf("Error %d: %s %s\n", errordesc[E].code, errordesc[E].message ,msg);
}

error_ECE576A_t ECE576A_HW5(
    const char* inputImageFileName,
    const char* outputImageFileName,
    uint8_t MaxOutputValue,
    uint8_t ComponentGrayLevel,
    bool verbose);

#endif /* HW5_H */
