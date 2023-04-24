#include "log.h"
#include "brightness.h"
#include "settings.h"
#include "utility.h"

#include <Adafruit_VEML7700.h>

#define LOG_TAG "brightness"

Adafruit_VEML7700 veml = Adafruit_VEML7700();
bool veml_setup = false;

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

    std::string str_min_brightness = get_setting("<system>", "Minimum Brightness", "20");
    std::string str_max_brightness = get_setting("<system>", "Maximum Brightness", "120");

    uint16_t min_brightness = 20;
    if (is_number(str_min_brightness)) min_brightness = std::stoi(str_min_brightness);

    uint16_t max_brightness = 120;
    if (is_number(str_max_brightness)) max_brightness = std::stoi(str_max_brightness);

    uint16_t als = veml.readALS();
    return map2(als, (uint16_t)0, (uint16_t)300, min_brightness, max_brightness);
}