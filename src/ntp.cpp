#include "wifi.h"
#include "rtc.h"
#include "settings.h"
#include "pins.h"
#include "log.h"

#include <WiFi.h>

#define LOG_TAG "ntp"

void get_time_from_ntp()
{
    if (is_wifi_connected())
    {
        logi(LOG_TAG, "Setting time using NTP");

        // get ntp settings
        std::string ntp_server_1 = get_setting("<ntp>", "NTP Server", "pool.ntp.org");
        long gmt_offset   = std::stoi(get_setting("<ntp>", "GMT Offset", "0"));
        long dst_offset   = std::stoi(get_setting("<ntp>", "DST Offset", "3600"));
        std::string tz    = get_setting("<ntp>", "POSIX Timezone", "GMT0BST,M3.5.0/1,M10.5.0");
        logi(LOG_TAG, "server=%s, gmt=%d, dst=%d", ntp_server_1.c_str(), gmt_offset, dst_offset);

        // get time from ntp
        configTime(gmt_offset, dst_offset, ntp_server_1.c_str());

        // set timezone (if a timezone has been specified)
        if (!tz.empty()) {
            logi(LOG_TAG, "setting timezone to %s", tz.c_str());
            setenv("TZ", tz.c_str(), 1);
            tzset();
        }

        // get time and write to rtc
        struct tm timeinfo;
        getLocalTime(&timeinfo);
        DateTime dt(timeinfo.tm_year - 100, timeinfo.tm_mon, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        set_rtc_time(dt);

        logi(LOG_TAG, "time: %02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    }
    else logw(LOG_TAG, "Can't use NTP client because i'm not connected to wifi");
}