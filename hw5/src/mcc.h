#ifndef MCC_H
#define MCC_H

#include "ECE576A.h"

void findMaximal8ConnectedForegroundComponents(IMAGE *img, uint8_t **componentMatrix);
void OverlayComponentsOntoImage(IMAGE *img, uint8_t **componentMatrix);

#endif /*MCC_H*/
