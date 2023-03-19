#ifndef CLOCK_SENSORS_H
#define CLOCK_SENSORS_H

#include <Adafruit_SHTC3.h>

extern Adafruit_SHTC3 sht;

void setup_sensors(TwoWire *wire = &Wire);

bool temp_available();
float get_temp();

bool humidity_available();
float get_humidity();

#endif /* CLOCK_SENSORS_H */