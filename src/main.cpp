#include <Arduino.h>
#include "pins.h"
#include "display.h"
#include "brightness.h"
#include "sensors.h"
#include "rtc.h"
#include "settings.h"
#include "sd.h"
#include "screens/screens.h"
#include "log.h"
#include <SPIFFS.h>

#define LOG_TAG "main"

uint32_t last_switch_time = 0;
uint32_t last_brightness_time = 0;
uint32_t last_random_time = 0;
int next_screen = 0;
int cycle_screens = 0;

void set_cycle_screens(std::string new_value) {
    cycle_screens = std::stoi(new_value.c_str());
}

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

    // setup screen subsystem and settings
    setup_screens();
    set_setting("<system>", "Cycle Screens", "0");
    register_setting_callback("<system>", "Cycle Screens", set_cycle_screens);
    set_setting("<system>", "City", "2655984"); // Belfast, Northern Ireland
    set_setting("<system>", "NTP Update Frequency", "86400000"); // 24 hours in milliseconds

    // load settings from sd card
    load_settings(SD);
    
    // start!
    switch_screen(1);
    display->setBrightness(255);
}

void loop()
{
    update_screen(display);

    // cycle screens
    if (cycle_screens)
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

    // randomise rainbow speed
    // if (millis() - last_random_time > 2500)
    // {
    //     uint16_t new_speed = 1 + rand() % 1000;
    //     set_setting("Digital Clock", "Rainbow Speed", std::to_string(new_speed).c_str());
    //     last_random_time = millis();
    // }

    // change gif
    // if (millis() - last_random_time > 2500)
    // {
    //     uint16_t new_gif = rand() % 5;

    //     switch(new_gif) {
    //         case 0:
    //             set_setting("GIF Player", "GIF File", "/gifs/amogus64.gif");
    //             break;
    //         case 1:
    //             set_setting("GIF Player", "GIF File", "/gifs/dipp.gif");
    //             break;
    //         case 2:
    //             set_setting("GIF Player", "GIF File", "/gifs/kirby.gif");
    //             break;
    //         case 3:
    //             set_setting("GIF Player", "GIF File", "/gifs/puppycat.gif");
    //             break;
    //         case 4:
    //             set_setting("GIF Player", "GIF File", "/gifs/squid.gif");
    //             break;
    //     }

    //     last_random_time = millis();
    // }
}