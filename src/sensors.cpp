#include "log.h"
#include "sensors.h"

#define LOG_TAG "sensors"

Adafruit_SHTC3 sht = Adafruit_SHTC3();
bool temp_setup = false;
bool humidity_setup = false;

void setup_sensors(TwoWire *wire)
{
    logi(LOG_TAG, "setting up shtc3");
    if (!sht.begin(wire)) {

        loge(LOG_TAG, "failed to setup shtc3");

    }
    else 
    {
        temp_setup = true;
        humidity_setup = true;
    }
}

bool temp_available() { return temp_setup; }
float get_temp() 
{
    if (!temp_setup) return 0.0;

    sensors_event_t temp, humidity;
    sht.getEvent(&humidity, &temp);

    return temp.temperature;

}

bool humidity_available() { return humidity_setup; }
float get_humidity()
{
    if (!humidity_setup) return 0.0;

    sensors_event_t temp, humidity;
    sht.getEvent(&humidity, &temp);

    return humidity.relative_humidity;
}