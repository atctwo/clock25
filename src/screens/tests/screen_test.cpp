#include "pins.h"
#ifdef INCLUDE_TEST_SCREENS

#include "../screens.h"
#include "log.h"

#define LOG_TAG "screen:test"

void ScreenTest::setup(Adafruit_GFX *display)
{
    logi(LOG_TAG, "Test!!!");
    this->display = display;
}

void ScreenTest::loop()
{
    this->display->setCursor(2, 2);
    this->display->setTextColor(0xFFFF);
    this->display->printf("hello :)\n");

    this->display->setTextColor(0x067A);
    this->display->printf("clock25 %c\n", 0x03);

    this->display->setTextColor(0xECDE);
    // logi(LOG_TAG, "setting up screens");
    // logi(LOG_TAG, "built-in screens:");
    for (ScreenInfo screen : screens)
    {
        // logi(LOG_TAG, "- %s", screen.name);
        this->display->printf("%s\n", screen.name);
    }
}

void ScreenTest::finish()
{
    logi(LOG_TAG, "Finish!!!");
}

void ScreenTest::setting_update(const char* setting, const char *new_setting)
{
    
}

#endif /* INCLUDE_TEST_SCREENS */