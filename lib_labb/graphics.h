#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "gpio.h"

void display_init(void);
void graphic_init(void); 
void graphic_clearScreen(void);
void pixel(uint32_t x, uint32_t y, uint32_t set);

#endif