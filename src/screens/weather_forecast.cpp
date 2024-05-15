#include "sensors.h"
#include "rtc.h"
#include "wifi.h"
#include "image.h"
#include "settings.h"
#include "screens.h"
#include "utility.h"
#include "log.h"
#include <WiFi.h>
#include <HTTPClient.h>

#include "../fonts/varela_round_5.h"

#define LOG_TAG "screen:weather_forecast"
#define FORECAST_ENTRIES 20
#define FORECAST_STEP 2

/*
    Values for status
    0: initial state
    1: getting weather
    2: got forecast
    3: failed to get weather because wifi is not connected
    4: failed to get weather because http request failed
    5: failed to get weather because deserialisation error
*/

void WeatherForecast::show_message(const char *msg, const char *title, bool newline)
{
    this->display->fillScreen(0);
    this->display->setCursor(0, 8);

    this->display->setTextColor(0x067A);
    this->display->println(title);
    this->display->setTextColor(0xffff);
    this->display->print(msg);

    if (newline) this->display->print("\n");
}

void WeatherForecast::get_forecast()
{
    logi(LOG_TAG, "getting 5 day weather forecast");
    this->status = 1;

    // assemble url string
    std::string request_url = "https://api.openweathermap.org/data/2.5/forecast?lat=";
    request_url += get_setting(nullptr, "Latitude", "54.59");
    request_url += "&lon=";
    request_url += get_setting(nullptr, "Longitude", "-5.93");
    request_url += "&appid=";
    request_url += get_setting(nullptr, "OpenWeatherMap API Key", "");

    HTTPClient client;
    client.begin(request_url.c_str());

    int response_code = client.GET();
    if (response_code) {

        logi(LOG_TAG, "HTTP response code: %d", response_code);
        // logi(LOG_TAG, "HTTP response: %s", client.getString().c_str());

        // create json document
        this->doc = new DynamicJsonDocument(JSON_DYNAMIC_BUFFER_SIZE);
        this->doc_allocated = true;

        // deserialise json document
        DeserializationError deserialisation_error = deserializeJson(*this->doc, client.getString());
        if (deserialisation_error)
        {
            logi(LOG_TAG, "deserialisation error: %s", deserialisation_error.f_str());
            show_message("JSON Read\nerror");
            this->status = 5;
        }
        else
        {
            logi(LOG_TAG, "showing weather for %s", (*this->doc)["city"]["name"].as<const char*>());
            show_message("", (*this->doc)["city"]["name"]);
            this->status = 2;

            JsonArray list = (*this->doc)["list"];
            logi(LOG_TAG, "list has %d entries", list.size());

            for (int i = 0; i < FORECAST_ENTRIES; i += FORECAST_STEP)
            {
                JsonObject entry = list[i];
                uint16_t y_pos = ((i/2) * 10) + 18; // y position of line


                // draw icon
                std::string icon_path = "/weather/";
                icon_path += entry["weather"][0]["icon"].as<const char*>();
                icon_path += ".bmp";
                draw_image(icon_path.c_str(), display, 1, y_pos-8);


                // get forecast time
                DateTime tm(entry["dt"].as<uint64_t>());

                display->setCursor(10, y_pos);
                display->setTextColor(0xffff);
                display->printf("%02d", tm.hour());


                // get forecasted temperature
                float temp = entry["main"]["temp"].as<float>() - 273.15; // kelvin to celcius

                // set text colour based on temperature
                float r, g, b;
                float temp_colour = map2(temp, (float)0, (float)30, (float)0, (float)1.0);
                getHeatMapColor(temp_colour, &r, &g, &b, {{0.0, 0.0, 1.0}, {1.0, 0.0, 0.0}});
                display->setTextColor(rgb888torgb565((uint8_t)(r*255.0), (uint8_t)(g*255.0), (uint8_t)(b*255.0)));

                display->setCursor(24, y_pos);
                display->printf("%2.0f%c", temp, 0xb0);


                // get forecasted wind speed
                float wind_speed = entry["wind"]["speed"].as<float>() * 2.237; // ms-1 to mph

                // set text colour based on wind speed
                float wind_speed_colour = map2(wind_speed, (float)0, (float)20, (float)0, (float)1.0);
                getHeatMapColor(wind_speed_colour, &r, &g, &b, {{0.321, 0.850, 1.0}, {1.0, 1.0, 1.0}});
                display->setTextColor(rgb888torgb565((uint8_t)(r*255.0), (uint8_t)(g*255.0), (uint8_t)(b*255.0)));

                display->setCursor(42, y_pos);
                display->printf("%2.1f", wind_speed, 0xb0);

                // print newline
                display->print("\n");
            }
        }

    } else {
        show_message("Failed to\nmake HTTP\nrequest");
        this->status = 4;
    }

    client.end();
}

void WeatherForecast::setup(Adafruit_GFX *display)
{
    logi(LOG_TAG, "setting up weather forecast");

    this->display = display;
    this->status = 0;
    this->doc_allocated = false;
    this->display->setFont(&VarelaRound_Regular5pt8b);

    // check wifi status
    if (!WiFi.isConnected())
    {
        show_message("Not connected\nto WiFi");
        this->status = 3;
    }
    else
    {
        // show loading screen
        show_message("Loading\nforecast");

        // get forecast
        get_forecast();
    }
}

void WeatherForecast::loop()
{
    // show_message("oh no");
}

void WeatherForecast::finish()
{
    // free memory of dynamicjsondocument
    if (doc_allocated) delete doc;
}

void WeatherForecast::setting_update(const char* setting, const char *new_setting)
{

}