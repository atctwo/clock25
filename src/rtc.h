#ifndef CLOCK_RTC_H
#define CLOCK_RTC_H

#include <RTClib.h>

bool setup_rtc(TwoWire *wire = &Wire);
bool rtc_available();
DateTime get_rtc_time();
void set_rtc_time(DateTime time);

#endif /* CLOCK_RTC_H */