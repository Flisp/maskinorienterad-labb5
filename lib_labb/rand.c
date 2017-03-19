#include "rand.h"
static uint32_t next = 1;

uint32_t rand(void)
{
    next = next * 1103515245 + 12345;
    return (uint32_t)(next/65536) % 32768;	
}

void srand(unsigned int seed) { next = seed; }