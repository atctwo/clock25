#include "pins.h"
#ifdef INCLUDE_TEST_SCREENS

#include "settings.h"
#include "utility.h"
#include "../screens.h"
#include "log.h"

#define LOG_TAG "screen:panel_test"

void PanelTest::setPanelColour(const char *new_colour) {

    logi(LOG_TAG, "Setting panel colour to %s", new_colour);

    // parse string as int
    int rgb888 = strtol(new_colour, NULL, 16);
    logi(LOG_TAG, "rgb888=%d", rgb888);

    // get r, g, b channels
    uint8_t r = (rgb888 & 0xff0000) >> 16;
    uint8_t g = (rgb888 & 0x00ff00) >> 8;
    uint8_t b = (rgb888 & 0x0000ff);
    logi(LOG_TAG, "r=%d, g=%d, b=%d", r, g, b);

    // convert to RGB565
    uint16_t rgb565 = rgb888torgb565(r, g, b);
    logi(LOG_TAG, "rgb565=%d", rgb565);

    // set panel colour
    this->display->fillScreen(rgb565);

}

void PanelTest::setup(Adafruit_GFX *display)
{
    logi(LOG_TAG, "Panel Test!!!");
    this->display = display;

    std::string colour = get_setting(nullptr, "Panel Colour", "00ced1");
    setPanelColour(colour.c_str());
}

void PanelTest::loop()
{
}

void PanelTest::finish()
{
    logi(LOG_TAG, "Finish!!!");
}

void PanelTest::setting_update(const char* setting, const char *new_setting)
{
    setPanelColour(new_setting);
}

#endif /* INCLUDE_TEST_SCREENS */