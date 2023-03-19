#include "log.h"
#include "brightness.h"

#include <Adafruit_VEML7700.h>

#define LOG_TAG "brightness"

Adafruit_VEML7700 veml = Adafruit_VEML7700();
bool veml_setup = false;


// modified version of arduino map that uses templates
// https://www.arduino.cc/reference/en/language/functions/math/map/
template<typename T>
T map2(T x, T in_min, T in_max, T out_min, T out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


bool setup_brightness(TwoWire *wire)
{
    logi(LOG_TAG, "setting up veml7700");

    if (!veml.begin(wire)) {

        logi(LOG_TAG, "veml7700 not detected");
        veml_setup = false;

    } else {

        logi(LOG_TAG, "veml7700 setup!");
        veml_setup = true;

    }

    return veml_setup;
}

bool brightness_available()
{
    return veml_setup;
}

uint8_t get_brightness()
{
    if (!veml_setup) return 0;

    uint16_t als = veml.readALS();
    return map2(als, (uint16_t)0, (uint16_t)300, (uint16_t)20, (uint16_t)255);
}