#include <Arduino.h>
#include "pins.h"
#include "display.h"
#include "brightness.h"
#include "sensors.h"
#include "rtc.h"
#include "sd.h"
#include "screens/screens.h"
#include "log.h"
#include <SPIFFS.h>

#define LOG_TAG "main"

void setup()
{
    delay(1000);
    Serial.begin(115200);
    USBSerial.begin(115200);

    logi(LOG_TAG, "clock25 starting!");

    // setup i2c
    Wire.begin(SDA_PIN, SCL_PIN);

    // setup SPI
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

    // setup spiffs
    if (!SPIFFS.begin()) loge(LOG_TAG, "failed to init spiffs");

    // setup display and peripherals
    setup_display();
    setup_brightness();
    setup_sensors();
    setup_rtc();
    setup_sd();
    switch_screen(0);

    display->setBrightness(255);
}


uint32_t last_switch_time = 0;
uint32_t last_brightness_time = 0;
int next_screen = 0;

void loop()
{
    update_screen(display);

    // cycle screens
    if (1)
    {
        if (millis() - last_switch_time > 5000)
        {
            next_screen++;
            if (next_screen >= screens.size()) next_screen = 0;

            switch_screen(next_screen);

            last_switch_time = millis();
        }
    }

    // update brightness
    if (millis() - last_brightness_time > 500) {
        if (brightness_available()) {

            display->setBrightness(get_brightness());
            last_brightness_time = millis();

        } else display->setBrightness(128);
    }
}