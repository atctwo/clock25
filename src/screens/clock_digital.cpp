#include "sensors.h"
#include "rtc.h"
#include "settings.h"
#include "screens.h"
#include "utility.h"
#include "log.h"

#include <string>
#include <WiFi.h>

#include "../fonts/open_sans_7.h"
#include "../fonts/open_sans_10.h"
#include "../fonts/open_sans_12.h"
#include "../fonts/varela_round_5.h"
#include "../fonts/varela_round_6.h"
#include "../fonts/varela_round_7.h"
#include "../fonts/varela_round_10.h"
#include "../fonts/varela_round_11.h"
#include "../fonts/varela_round_12.h"

#define LOG_TAG "screen:clock_digital"

GFXcanvas1 *canvas_time;


// icons for showing wifi signal strength
// '1', 7x5px
const unsigned char wifi_icon_1 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x80, 0x80
};
// '2', 7x5px
const unsigned char wifi_icon_2 [] PROGMEM = {
	0x00, 0x00, 0x20, 0x20, 0x20
};
// '3', 7x5px
const unsigned char wifi_icon_3 [] PROGMEM = {
	0x00, 0x08, 0x08, 0x08, 0x08
};
// '4', 7x5px
const unsigned char wifi_icon_4 [] PROGMEM = {
	0x02, 0x02, 0x02, 0x02, 0x02
};

int wifi_strength_w = 7;
int wifi_strength_h = 5;
int wifi_strength_x = PANEL_RES_X - wifi_strength_w - 1;
int wifi_strength_y = 0; //PANEL_RES_Y - wifi_strength_h;
uint16_t wifi_colour_active = 0xffff;
uint16_t wifi_colour_inactive = 0x52AA;



void drawRainbowBitmap(Adafruit_GFX *display, int16_t x, int16_t y,
  const uint8_t bitmap[], int16_t w, int16_t h,
  uint16_t bg, int phase_difference) {

    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

	float colour_parts = 360 / w;

    display->startWrite();
    for(int16_t j=0; j<h; j++, y++) {
        for(int16_t i=0; i<w; i++ ) {
            float R, G, B;
            HSVtoRGB(&R, &G, &B, fmodf(((((float) i) * colour_parts) + phase_difference), (float)360.0), 1.0, 1.0);
            if(i & 7) byte <<= 1;
            else      byte   = pgm_read_byte(&bitmap[j * byteWidth + i / 8]);
            display->writePixel(x+i, y, (byte & 0x80) ? rgb888torgb565((int)(R*255.0), (int)(G*255.0), (int)(B*255.0)) : bg);
        }
    }
    display->endWrite();
}





void ClockDigital::setup(Adafruit_GFX *display)
{
    this->display = display;
    canvas_time = new GFXcanvas1(PANEL_RES_X, 20);

    // get settings
    std::string speed = get_setting(nullptr, "Rainbow Speed", "14000");
    this->rainbow_speed = std::stoi(speed.c_str());
}

uint32_t last_time_update = 0;

void ClockDigital::loop()
{
    // if (millis() - last_time_update > 0)
    // {
        // display->fillScreen(0);

        if (rtc_available())
        {
            DateTime now = get_rtc_time();

            // print time
            canvas_time->fillScreen(0);
            canvas_time->setCursor(0, 18);
            canvas_time->setFont(&VarelaRound_Regular11pt7b);
            canvas_time->printf("%02d:%02d\n",
                now.hour(),
                now.minute()
            );
            drawRainbowBitmap(display, 0, 4, canvas_time->getBuffer(), PANEL_RES_X, 20, 0, (int)((float)(millis() % this->rainbow_speed) * (360 / (float)this->rainbow_speed)));

            // print date
            display->setFont(&VarelaRound_Regular7pt7b);
            display->setTextColor(0xffff, 0);
            display->setCursor(0, 39);
            display->printf("%02d.%02d.%02d\n",
                now.day(),
                now.month(),
                now.year() - 2000
            );

            // print seconds
            this->display->setFont(&VarelaRound_Regular5pt8b);
            display->setTextColor(0xAD55, 0);
            display->setCursor(0, 54);
            display->printf(" %02d %2.0f%c %2.0f%%",
                now.second(),
                get_temp(), 
                0xb0,   // ° symbol
                get_humidity()
            );

            // show wifi signal strength
            // display->setCursor(0, 10);
            // display->setTextColor(0xffff);
            // display->printf("rssi: %d", WiFi.RSSI());

            uint16_t wifi_colour_1 = (WiFi.RSSI() > -70 && WiFi.isConnected()) ? wifi_colour_active : wifi_colour_inactive;
            display->drawBitmap(wifi_strength_x, wifi_strength_y, wifi_icon_1, wifi_strength_w, wifi_strength_h, wifi_colour_1);

            uint16_t wifi_colour_2 = (WiFi.RSSI() > -67 && WiFi.isConnected()) ? wifi_colour_active : wifi_colour_inactive;;
            display->drawBitmap(wifi_strength_x, wifi_strength_y, wifi_icon_2, wifi_strength_w, wifi_strength_h, wifi_colour_2);

            uint16_t wifi_colour_3 = (WiFi.RSSI() > -55 && WiFi.isConnected()) ? wifi_colour_active : wifi_colour_inactive;;
            display->drawBitmap(wifi_strength_x, wifi_strength_y, wifi_icon_3, wifi_strength_w, wifi_strength_h, wifi_colour_3);

            uint16_t wifi_colour_4 = (WiFi.RSSI() > -30 && WiFi.isConnected()) ? wifi_colour_active : wifi_colour_inactive;;
            display->drawBitmap(wifi_strength_x, wifi_strength_y, wifi_icon_4, wifi_strength_w, wifi_strength_h, wifi_colour_4);

            last_time_update = millis();
        }
        else 
        {
            display->setCursor(0xffff, 0);
            display->println("\n\n\n RTC not\n Available");
        }

    // }


}

void ClockDigital::finish()
{
    delete canvas_time;
}

void ClockDigital::setting_update(const char* setting, const char *new_setting)
{
    // update rainbow speed
    if (strcmp(setting, "Rainbow Speed") == 0) {
        this->rainbow_speed = std::stoi(new_setting);
    }
}