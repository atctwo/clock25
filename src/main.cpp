#include <Arduino.h>
#include "pins.h"
#include "display.h"
#include "brightness.h"
#include "sensors.h"
#include "rtc.h"
#include "gesture.h"
#include "settings.h"
#include "sd.h"
#include "wifi.h"
#include "ntp.h"
#include "server.h"
#include "screens/screens.h"
#include "log.h"
#include <SPIFFS.h>

#define LOG_TAG "main"

uint32_t last_switch_time = 0;
uint32_t last_brightness_time = 0;
uint32_t last_random_time = 0;
bool screen_blanked = 0;
uint8_t last_brightness = get_brightness();

uint32_t last_ntp_update = 0;
uint32_t ntp_update_frequency = 0;
bool enable_automatic_ntp = true;

uint32_t last_gesture_check = 0;
uint32_t gesture_check_frequency = 0;

int next_screen = 0;
int cycle_screens = 0;

void set_cycle_screens(std::string new_value) {
    cycle_screens = std::stoi(new_value.c_str());
}

void set_ntp_update_frequency(std::string new_value) {
    last_ntp_update = millis(); // reset ntp timer
    ntp_update_frequency = std::stoi(new_value.c_str());
}

void set_automatic_ntp_update(std::string new_value)
{
    enable_automatic_ntp = std::stoi(new_value.c_str());
}

void set_screen_blanked(std::string new_value)
{
    screen_blanked = std::stoi(new_value);

    if (screen_blanked) {
        last_brightness = get_brightness();
        set_display_brightness(1);
    } else {
        set_display_brightness(last_brightness);
    }
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
    register_setting_callback("<system>", "Cycle Screens", set_cycle_screens);
    register_setting_callback("<system>", "Blank Screen", set_screen_blanked);
    register_setting_callback("<ntp>", "NTP Update Frequency", set_ntp_update_frequency);
    register_setting_callback("<ntp>", "Enable automatic NTP", set_ntp_update_frequency);
    set_setting_values("<system>", "Cycle Screens", {"0", "1"});
    set_setting_values("<system>", "Blank Screen", {"0", "1"});
    set_setting_values("<ntp>", "Enable automatic NTP", {"0", "1"});

    // setup gesture (after screens so it can use screens to set setting values)
    setup_gesture();

    // load settings from sd card
    if (sd_available()) load_settings(SD);
    else logi(LOG_TAG, "Can't derserialise settings from SD because SD isn't available");
    
    // setup wifi
    setup_wifi();
    connect_wifi([](){
        // only do these thing when wifi is connected
        get_time_from_ntp();
        setup_ntp_server();
        setup_http_server();
    });

    // start!
    switch_screen(0);
    set_display_brightness(128);
}

void loop()
{
    update_screen(display);
    update_fade();

    // handle server requests
    server_handle_client();

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
    if (!screen_blanked)
    {
        if (millis() - last_brightness_time > 500) {
            if (brightness_available()) {

                set_display_brightness(get_brightness());
                last_brightness_time = millis();

            } else set_display_brightness(128);
        }
    }

    // ntp update
    if (enable_automatic_ntp && ntp_update_frequency != 0) {
        if (millis() - last_ntp_update > ntp_update_frequency) {
            get_time_from_ntp();
            last_ntp_update = millis();
        }
    }

    // check gesture
    if (millis() - last_gesture_check > gesture_check_frequency)
    {
        check_gesture();
        last_gesture_check = millis();
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