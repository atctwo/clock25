#include "wifi.h"
#include "settings.h"
#include "pins.h"
#include "log.h"

#include <WiFi.h>

#define LOG_TAG "wifi"
#define CONNECT_TIMEOUT 10000 // 10 seconds
bool wifi_setup = false;
std::string psk = "none";
std::string ssid = "none";
wifi_connect_cb_t wifi_connect_cb = nullptr;

void wifi_event_callback(WiFiEvent_t event)
{
    switch(event)
    {
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            logi(LOG_TAG, "Connected to %s!", ssid.c_str());
            break;

        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            logi(LOG_TAG, "Disconnected from wifi");
            wifi_setup = false;
            break;

        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            logi(LOG_TAG, "Got IPv4 address: %s!", WiFi.localIP().toString().c_str());
            wifi_setup = true;

            // run wifi connect callback
            if (wifi_connect_cb) {
                wifi_connect_cb();
            }

            break;

        default:
            logi(LOG_TAG, "other wifi event: %d", event);
    }
}


void setup_wifi()
{
    logi(LOG_TAG, "setting up wifi");

    WiFi.setHostname("clock25");        // set hostname
    WiFi.mode(WIFI_STA);                // set wifi to station mode
    WiFi.onEvent(wifi_event_callback);  // register wifi event callbacks
}

bool connect_wifi(wifi_connect_cb_t callback)
{
    psk  = get_setting("<wifi>", "psk", "");
    ssid = get_setting("<wifi>", "ssid", "");

    logi(LOG_TAG, "Connecting to wifi (ssid \"%s\")", ssid.c_str());

    // if ssid is blank
    if (ssid[0] == 0) {
        loge(LOG_TAG, "Can't connect to wifi, SSID is blank");
        wifi_setup = false;
        wifi_connect_cb = nullptr;
    }
    else
    {
        wifi_connect_cb = callback;
        WiFi.begin(ssid.c_str(), psk.c_str());

        uint32_t connect_start_time = millis();
        bool timed_out = false;
        while(WiFi.status() == WL_DISCONNECTED)
        {
            // check timeout
            if (millis() - connect_start_time > CONNECT_TIMEOUT)
            {
                loge(LOG_TAG, "Wifi connection timed out");
                timed_out = true;
                wifi_setup = false;
                wifi_connect_cb = nullptr;
                break;
            }
        }
    }

    return wifi_setup;
}

bool is_wifi_connected() { return wifi_setup; }