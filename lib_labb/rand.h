#ifndef RAND_H
#define RAND_H

#include "types.h"

#define RAND_MAX 32767

uint32_t rand(void);
void srand(unsigned int seed);

#endif
