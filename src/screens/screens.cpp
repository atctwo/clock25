#include "screens.h"
#include "display.h"
#include "settings.h"
#include "pins.h"
#include "log.h"

#define LOG_TAG "screens"
#define SCREEN_FADE_TIME 250

//-----------------------------------------------------
// Screen vector initialisation
//-----------------------------------------------------

std::vector<ScreenInfo> screens = {

    {"Digital Clock", make<ClockDigital>, {
        {"Rainbow Speed", "14000"}
    }, true, true},

    {"GIF Player", make<GIFPlayer>, {
        {"GIF File", "/gifs/katamari.gif"}
    }, false, false},

    {"Weather Forecast", make<WeatherForecast>, {
    }, false, true},

    #ifdef INCLUDE_TEST_SCREENS
        {"TestScreen", make<ScreenTest>, {}, true, true},
        {"Test B", make<ScreenTestB>, {}, true, true},
        {"Panel Test", make<PanelTest>, {
            {"Panel Colour", "00ced1"}
        }, false, true},
        {"Brightness", make<BrightnessTest>, {}, true, true},
        {"Sensors", make<SensorTest>, {}, true, true},
        {"SD Test", make<SDTest>, {}, true, true},
    #endif

};




//-----------------------------------------------------
// Screen init and current screen reference
//-----------------------------------------------------

ScreenBase *current_screen = NULL;       // reference to object for the currently loaded screen
int current_screen_id = -1;              // id of the current screen's class creator in the `screens` vector
// int next_screen_id = -1;                 // internal flag + id for which screen to switch to
bool switching_screen = false;           // whether the screen is currently being switched
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

bool switch_screen(int new_screen_id)
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
        return false;
    }

    return true;
}

void next_screen()
{
    logi(LOG_TAG, "switching!");

    // if there is already a screen running
    if (current_screen)
    {
        // call finish function for current screen
        current_screen->finish();

        // clean up current screen
        delete current_screen;
    }

    // clear screen and reset font
    canvas.fillScreen(0);
    canvas.setTextColor(0xffff, 0);
    canvas.setCursor(0, 0);
    canvas.setFont(NULL);

    // instantiate new screen
    current_screen = screens[next_screen_id].creator();
    current_screen_id = next_screen_id;

    // set up new screen
    // current_screen->setup(display);
    current_screen->setup(&canvas);

    // clear next_screen_id
    next_screen_id = -1;
    switching_screen = false;

    // fade display back in
    fade_display(get_display_brightness(), nullptr, SCREEN_FADE_TIME, true);
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
    // else { logi(LOG_TAG, "no screen loaded :/"); }

    // check if we're switching to a new screen
    // and that the switching screen flag isn't already set
    if (next_screen_id != -1 && !switching_screen)
    {
        logi(LOG_TAG, "beginning switch to screen %d", next_screen_id);

        // set the switching screen flag
        switching_screen = true;

        // asynchronously fade out
        fade_display(0, next_screen, SCREEN_FADE_TIME, true);        
    }
}

const char *current_screen_name()
{
    return current_screen ? screens[current_screen_id].name : "No screen loaded";
}

std::vector<std::string> get_screen_names()
{
    std::vector<std::string> names;

    for (ScreenInfo screen : screens)
    {
        names.push_back(screen.name);
    }

    return names;
}