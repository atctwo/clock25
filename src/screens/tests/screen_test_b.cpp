#include "pins.h"
#ifdef INCLUDE_TEST_SCREENS

#include "../screens.h"
#include "log.h"

#define LOG_TAG "screen:testb"

void ScreenTestB::setup(Adafruit_GFX *display)
{
    logi(LOG_TAG, "Test!!!");
    this->display = display;
}

void ScreenTestB::loop()
{
    this->display->setCursor(2, 2);

    this->display->setTextColor(0x067A);
    this->display->printf("clock25 %c\n", 0x03);

    this->display->setTextColor(0xECDE);
    this->display->printf("Test B\n");


    this->display->setTextColor(0, 0x5E7F);
    this->display->println("  TRANS   ");

    this->display->setTextColor(0, 0xF557);
    this->display->println("  RIGHTS  ");

    this->display->setTextColor(0, 0xffff);
    this->display->println("  ARE     ");

    this->display->setTextColor(0, 0xF557);
    this->display->println("  HUMAN   ");

    this->display->setTextColor(0, 0x5E7F);
    this->display->println("  RIGHTS  ");
}

void ScreenTestB::finish()
{
    logi(LOG_TAG, "Finish!!!");
}

#endif /* INCLUDE_TEST_SCREENS */