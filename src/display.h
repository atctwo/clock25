#ifndef CLOCK_DISPLAY_H
#define CLOCK_DISPLAY_H

/*
 * objects and functions to drive the LED matrix
 * using https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-DMA 
*/

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// The LED matrix display object
extern MatrixPanel_I2S_DMA *display;

/**
 * Function to create and configure the display.
 * This should be called before `display` is used.
*/
void setup_display();

#endif /* CLOCK_DISPLAY_H */