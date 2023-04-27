#include "button.h"
#include "rtc.h"
#include "screens/screens.h"
#include "settings.h"
#include "utility.h"
#include "pins.h"
#include "log.h"

#define LOG_TAG "button"
#define BTN_REPEAT_DELAY 600
#define BTN_REPEAT_RATE  100

uint32_t last_btn_hour_read   = 0;
uint32_t last_btn_min_read    = 0;
uint32_t last_btn_screen_read = 0;

uint32_t last_btn_hour_repeat   = 0;
uint32_t last_btn_min_repeat    = 0;
uint32_t last_btn_screen_repeat = 0;

bool btn_hour_pressed   = false;
bool btn_min_pressed    = false;
bool btn_screen_pressed = false;

void setup_buttons()
{
    logi(LOG_TAG, "setting up buttons");
    pinMode(BTN_HOUR, INPUT);
    pinMode(BTN_MINUTE, INPUT);
    pinMode(BTN_SCREEN, INPUT);
    pinMode(BTN_REVERSE, INPUT);
}

void adjust_time(bool btn_hour, bool btn_min, bool btn_reverse)
{
    // get current time
    DateTime tm = get_rtc_time();
    uint64_t unixtime = tm.unixtime();

    // adjust time based on button(s) pressed
    // if btn_reverse is pressed, multiply the time to add by -1 so it subtracts
    if (btn_hour) unixtime += 3600 * (btn_reverse ? -1 : 1);
    if (btn_min)  unixtime += 60   * (btn_reverse ? -1 : 1);

    // set time
    DateTime tm2(unixtime);
    set_rtc_time(tm2);
}

void adjust_screen(bool btn_reverse)
{
    // get id of screen to switch to
    int next_screen = current_screen_id + (btn_reverse ? -1 : 1);

    // check id is in bounds of existing screens
    if (next_screen < 0) {
        next_screen = screens.size() - 1;
    }
    else if (next_screen >= screens.size()) {
        next_screen = 0;
    }

    // switch screen
    switch_screen(next_screen);
}

void check_buttons()
{
    // check buttons
    bool btn_hour    = digitalRead(BTN_HOUR);
    bool btn_min     = digitalRead(BTN_MINUTE);
    bool btn_screen  = digitalRead(BTN_SCREEN);
    bool btn_reverse = digitalRead(BTN_REVERSE);

    // check against known state
    if (btn_hour && !btn_hour_pressed) {
        adjust_time(true, false, btn_reverse);
        btn_hour_pressed = true;
        last_btn_hour_read = millis();
    }

    if (btn_min && !btn_min_pressed) {
        adjust_time(false, true, btn_reverse);
        btn_min_pressed = true;
        last_btn_min_read = millis();
    }

    if (btn_screen && !btn_screen_pressed) {
        adjust_screen(btn_reverse);
        btn_screen_pressed = true;
        last_btn_screen_read = millis();
    }

    // check button repeat
    if (btn_hour_pressed)
    {
        if (millis() - last_btn_hour_read > BTN_REPEAT_DELAY)
        {
            if (millis() - last_btn_hour_repeat > BTN_REPEAT_RATE)
            {
                adjust_time(true, false, btn_reverse);
                last_btn_hour_repeat = millis();
            }
        }
    }

    if (btn_min_pressed)
    {
        if (millis() - last_btn_min_read > BTN_REPEAT_DELAY)
        {
            if (millis() - last_btn_min_repeat > BTN_REPEAT_RATE)
            {
                adjust_time(false, true, btn_reverse);
                last_btn_min_repeat = millis();
            }
        }
    }

    // if (btn_screen_pressed)
    // {
    //     if (millis() - last_btn_screen_read > BTN_REPEAT_DELAY)
    //     {
    //         if (millis() - last_btn_screen_repeat > BTN_REPEAT_RATE)
    //         {
    //             adjust_screen(btn_reverse);
    //             last_btn_screen_repeat = millis();
    //         }
    //     }
    // }

    // if button is no longer held, clear pressed state
    if (!btn_hour)   btn_hour_pressed = false;
    if (!btn_min)    btn_min_pressed = false;
    if (!btn_screen) btn_screen_pressed = false;

}