#ifndef MCC_H
#define MCC_H

#include "ECE576A.h"
#include <stdint.h>
#include <stdbool.h>

void findMaximal8ConnectedForegroundComponents(IMAGE *img, uint8_t **outccM, 
    bool CGL, int *nc, bool verbose);
void OverlayComponentsOntoImage(IMAGE *img, uint8_t **componentMatrix, int nc,
    bool CGL, bool MOV);

#endif /*MCC_H*/
