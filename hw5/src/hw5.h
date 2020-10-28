#ifndef HW5_H
#define HW5_H

void printECE576AHW5Help();

error_ECE576A_t ECE576A_HW5(
    const char* inputImageFileName, 
    const char* outputImageFileName, 
    uint8_t MaxOutputValue, 
    uint8_t ComponentGrayLevel);

#endif /* HW5_H */