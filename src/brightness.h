#ifndef CLOCK_BRIGHTNESS_H
#define CLOCK_BRIGHTNESS_H

#include <stdint.h>
#include <Adafruit_VEML7700.h>

extern Adafruit_VEML7700 veml;

bool setup_brightness(TwoWire *wire = &Wire);
bool brightness_available();
uint8_t get_brightness();

#endif /* CLOCK_BRIGHTNESS_H */