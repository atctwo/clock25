#include "screens.h"
#include "settings.h"
#include "pins.h"
#include "log.h"

#define LOG_TAG "screens"


//-----------------------------------------------------
// Screen vector initialisation
//-----------------------------------------------------

std::vector<ScreenInfo> screens = {

#ifdef INCLUDE_TEST_SCREENS
    {"TestScreen", make<ScreenTest>, {}, true},
    {"Test B", make<ScreenTestB>, {}, true},
    {"Brightness", make<BrightnessTest>, {}, true},
    {"Sensors", make<SensorTest>, {}, true},
    {"SD Test", make<SDTest>, {}, true},
#endif

    {"Digital Clock", make<ClockDigital>, {
        {"Rainbow Speed", "15"}
    }, true},

    {"GIF Player", make<GIFPlayer>, {
        {"GIF File", "/gifs/katamari.gif"}
    }, false}

};




//-----------------------------------------------------
// Screen init and current screen reference
//-----------------------------------------------------

ScreenBase *current_screen = NULL;       // reference to object for the currently loaded screen
int current_screen_id = -1;              // id of the current screen's class creator in the `screens` vector
// int next_screen_id = -1;                 // internal flag + id for which screen to switch to
GFXcanvas16 canvas(PANEL_RES_X, PANEL_RES_Y);

void setup_screens()
{
    logi(LOG_TAG, "registering screen settings");

    // for each screen
    for (ScreenInfo screen : screens)
    {
        // for each setting
        for (ScreenSettings setting : screen.settings)
        {
            // set setting to default
            // this registers the setting so the system knows about it
            // the value saved on the sd card will be loaded later
            set_setting(screen.name, setting.name, setting.default_value);
        }
    }

    logi(LOG_TAG, "setup screens!");
}

void switch_screen(int new_screen_id)
{
    if (new_screen_id >= 0 && new_screen_id < screens.size())
    {
        logi(LOG_TAG, "registered screen switch to screen %d (%s)", 
            new_screen_id, 
            screens[new_screen_id].name
        );

        // set variable to indicate to switch to new state after
        // current state's loop() is over
        next_screen_id = new_screen_id;
    } 
    else
    {
        loge(LOG_TAG, "invalid screen id (no screen %d)", new_screen_id);
    }
}

void update_screen(Adafruit_GFX *display)
{
    if (current_screen)
    {
        // clear canvas
        if (screens[current_screen_id].clear_buffer) canvas.fillScreen(0);

        // call loop function for current screen
        current_screen->loop();

        // draw canvas
        display->drawRGBBitmap(0, 0, canvas.getBuffer(), PANEL_RES_X, PANEL_RES_Y);
    }
    else { logi(LOG_TAG, "no screen loaded :/"); }

    // check if we're switching to a new screen
    if (next_screen_id != -1)
    {
        logi(LOG_TAG, "switching screen to screen %d", next_screen_id);

        // if there is already a screen running
        if (current_screen)
        {
            // call finish function for current screen
            current_screen->finish();

            // clean up current screen
            delete current_screen;
        }

        // clear screen
        display->fillScreen(0);
        display->setTextColor(0xffff, 0);

        // instantiate new screen
        current_screen = screens[next_screen_id].creator();
        current_screen_id = next_screen_id;

        // set up new screen
        // current_screen->setup(display);
        current_screen->setup(&canvas);

        // clear next_screen_id
        next_screen_id = -1;

        
    }
}

const char *current_screen_name()
{
    return current_screen ? screens[current_screen_id].name : "No screen loaded";
}