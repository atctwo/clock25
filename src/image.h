#ifndef CLOCK_IMAGE_H
#define CLOCK_IMAGE_H

#include "pins.h"
#include <FS.h>
#include <SD.h>
#include <Adafruit_GFX.h>

bool draw_image(const char *filename, Adafruit_GFX *display, uint16_t x, uint16_t y, FS fs=FILESYSTEM);

#endif /* CLOCK_IMAGE_H */