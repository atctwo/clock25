// adapted from adafruit's PCF8523 example: https://github.com/adafruit/RTClib/blob/master/examples/pcf8523/pcf8523.ino


#include "pins.h"
#include "log.h"
#include "rtc.h"

#define LOG_TAG "rtc"

RTC_PCF8523 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
bool rtc_setup = false;

bool setup_rtc(TwoWire *wire)
{
    logi(LOG_TAG, "setting up rtc");

    // begin rtc
    if (!rtc.begin(wire)) {

        // reset and try again
        rtc.reset();
        if (!rtc.begin(wire))
        {
            loge(LOG_TAG, "failed to setup rtc");
            rtc_setup = false;
            return false;
        }
    }

    // stop rtc
    rtc.stop();

    // set rtc time if not already set
    if (! rtc.initialized() || rtc.lostPower()) {
        logi(LOG_TAG, "setting initial rtc time");

        // allow rtc oscillator to stabalise
        delay(1000);

        // set time to compile time
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    // start rtc
    rtc.start();

    rtc_setup = true;
    return true;

}

bool rtc_available() { return rtc_setup; }

DateTime get_rtc_time()
{
    return rtc.now();
}

void set_rtc_time(DateTime time)
{
    rtc.adjust(time);
}