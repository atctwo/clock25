#include "gesture.h"
#include "settings.h"
#include "screens/screens.h"
#include "utility.h"
#include "pins.h"
#include "log.h"

#include <algorithm>
#include <Adafruit_APDS9960.h>

#define LOG_TAG "gesture"

Adafruit_APDS9960 apds;
bool gesture_setup = false;
int previous_state = 0;

bool setup_gesture()
{
    logi(LOG_TAG, "Setting up APDS9960");

    // set up apds9960
    if (!apds.begin()) {
        logw(LOG_TAG, "failed to setup apds9960");
        gesture_setup = false;
    } else {
        apds.enableProximity(true);
        apds.enableGesture(true);
        // apds.setGestureProximityThreshold(25);
        // apds.setGestureGain(APDS9960_GGAIN_8);
        gesture_setup = true;
    }

    // set values for settings
    set_setting_values("<system>", "Gesture Enabled", {"0", "1"});
    set_setting_values("<system>", "Gesture Alt Screen", get_screen_names());

    return gesture_setup;
}

void check_gesture()
{
    if (gesture_setup)
    {
        uint8_t gesture = apds.readGesture();
        if (gesture == APDS9960_UP)    logi(LOG_TAG, "up!");
        if (gesture == APDS9960_DOWN)  logi(LOG_TAG, "down!");
        if (gesture == APDS9960_LEFT)  logi(LOG_TAG, "left!");
        if (gesture == APDS9960_RIGHT) logi(LOG_TAG, "right!");

        if (gesture == APDS9960_LEFT || gesture == APDS9960_RIGHT)
        {
            // get gesture settings
            std::vector<std::string> screen_names = get_screen_names();
            std::vector<std::string>::iterator alt_screen_it = std::find(screen_names.begin(), screen_names.end(), get_setting("<system>", "Gesture Alt Screen", "Weather Forecast"));
            int alt_screen = (alt_screen_it != screen_names.end()) ? (alt_screen_it - screen_names.begin()) : 0; // cursed
            
            std::string str_gesture_enabled = get_setting("<system>", "Gesture Enabled", "1");
            bool gesture_enabled = true;
            if (is_number(str_gesture_enabled))  gesture_enabled = std::stoi(str_gesture_enabled);

            // switch screen
            if (gesture_enabled)
            {
                if (current_screen_id != alt_screen) {
                    previous_state = current_screen_id;
                    switch_screen(alt_screen);
                } else {
                    switch_screen(previous_state);
                }
            }
        }
    }
}

bool gesture_available() { return gesture_setup; }