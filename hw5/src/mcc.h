#ifndef MCC_H
#define MCC_H

#include "ECE576A.h"

error_ECE576A_t findMaximal8ConnectedForegroundComponents(IMAGE img, uint8_t **componentMatrix);
error_ECE576A_t OverlayComponentsOntoImage(IMAGE img, uint8_t **componentMatrix);

#endif /*MCC_H*/
