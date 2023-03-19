#include "pins.h"
#ifdef INCLUDE_TEST_SCREENS

#include "../screens.h"
#include "brightness.h"
#include "log.h"

#define LOG_TAG "screen:brightness_test"

void BrightnessTest::setup(Adafruit_GFX *display)
{
    logi(LOG_TAG, "Test!!!");
    this->display = display;
}

uint32_t last_time = 0;

void BrightnessTest::loop()
{

    this->display->setCursor(2, 2);
    this->display->setTextColor(0x067A, 0);
    this->display->printf("Brightness\n");

    if (!brightness_available()) {
        this->display->setTextColor(0xffff);
        this->display->println("\nFailed to\nsetup\nVEML7700");
    }

    if (brightness_available()) {

        // print brightness
        display->setCursor(0, 10);
        display->setTextColor(0xffff, 0);

        display->printf("ALS: %5d\n", veml.readALS());
        display->printf("W:   %5d\n", veml.readWhite());
        display->printf("Lux: \n%f       \n", veml.readLux());

        // reset timer
        last_time = millis();
    }
}

void BrightnessTest::finish()
{
    logi(LOG_TAG, "Finish!!!");
}

#endif /* INCLUDE_TEST_SCREENS */