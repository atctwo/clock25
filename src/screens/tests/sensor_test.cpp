#include "pins.h"
#ifdef INCLUDE_TEST_SCREENS

#include "../screens.h"
#include "sensors.h"
#include "log.h"

#define LOG_TAG "screen:sensor_test"

void SensorTest::setup(Adafruit_GFX *display)
{
    logi(LOG_TAG, "Test!!!");
    this->display = display;
}

uint32_t last_time_sensor = 0;

void SensorTest::loop()
{
    this->display->setCursor(2, 2);
    this->display->setTextColor(0x067A, 0);
    this->display->printf("Sensors\n");

    // every second
    // if (millis() - last_time_sensor > 200) {

        // setup cursor
        display->setCursor(0, 10);
        display->setTextColor(0xffff, 0);

        // print temperature
        if (temp_available()) {
            display->printf("Temp:%2.2f%c\n", get_temp(), 0xf8);
        } else {
            display->println("No temp");
        }

        // print humidity
        if (humidity_available()) {
            display->printf("RH: %3.2f%c\n", get_humidity(), 0x25);
        } else {
            display -> println("No RH");
        }

        // reset timer
        // last_time_sensor = millis();

    // }

}

void SensorTest::finish()
{
    logi(LOG_TAG, "Finish!!!");
}

#endif /* INCLUDE_TEST_SCREENS */