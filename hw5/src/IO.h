#ifndef IO_H
#define IO_H

#include <stdint.h>
#include "../libdip/src/dip.h"
#include "../libdip/src/pngio.h"
#include "ECE576A.h"

/* @brief writePNG writes a png to a file
 * @param raw_data uint8_t pseudo 2-D array created by matalloc
 * @param n_rows number of rows of the pseudo 2D array
 * @param n_cols number of columns of the pseudo 2D array
 */
void writePNG(uint8_t** raw_data, char* filename, int n_rows, int n_cols);

void readPNGandClose(char *input_file, IMAGE *img);

#endif
