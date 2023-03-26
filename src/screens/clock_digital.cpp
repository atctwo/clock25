#include "sensors.h"
#include "rtc.h"
#include "settings.h"
#include "screens.h"
#include "log.h"

#include <string>

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

// convert a 24 bit colour (RGB888) to a 16 bit colour (RGB565)
// https://afterthoughtsoftware.com/posts/convert-rgb888-to-rgb565
uint16_t rgb888torgb565(uint8_t red, uint8_t green, uint8_t blue)
{
    uint16_t b = (blue >> 3) & 0x1f;
    uint16_t g = ((green >> 2) & 0x3f) << 5;
    uint16_t r = ((red >> 3) & 0x1f) << 11;

    return (uint16_t) (r | g | b);
}

// convert a colour in HSV space to RGB
// from https://www.cs.rit.edu/~ncs/color/t_convert.html#RGB%20to%20HSV%20&%20HSV%20to%20RGB
void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
{
    int i;
    float f, p, q, t;
    if( s == 0 ) {
        // achromatic (grey)
        *r = *g = *b = v;
        return;
    }
    h /= 60;			// sector 0 to 5
    i = floor( h );
    f = h - i;			// factorial part of h
    p = v * ( 1 - s );
    q = v * ( 1 - s * f );
    t = v * ( 1 - s * ( 1 - f ) );
    switch( i ) {
        case 0:
            *r = v;
            *g = t;
            *b = p;
            break;
        case 1:
            *r = q;
            *g = v;
            *b = p;
            break;
        case 2:
            *r = p;
            *g = v;
            *b = t;
            break;
        case 3:
            *r = p;
            *g = q;
            *b = v;
            break;
        case 4:
            *r = t;
            *g = p;
            *b = v;
            break;
        default:		// case 5:
            *r = v;
            *g = p;
            *b = q;
            break;
    }
}

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
    std::string speed = get_setting(nullptr, "Rainbow Speed", "15");
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
            drawRainbowBitmap(display, 0, 4, canvas_time->getBuffer(), PANEL_RES_X, 20, 0, ((millis() % 60000) * this->rainbow_speed)/1000);

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