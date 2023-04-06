#ifndef CLOCK_SCREENS_H
#define CLOCK_SCREENS_H

/*
hmm
*/

#include <memory>
#include <vector>
#include <string>
#include <Adafruit_GFX.h>
#include "pins.h"


//-----------------------------------------------------
// Screen classes
//-----------------------------------------------------


// Base class from which all Screen classes are extended
class ScreenBase {
    public:
        virtual void setup(Adafruit_GFX *display) = 0;
        virtual void loop();
        virtual void finish();
        virtual void setting_update(const char* setting, const char *new_setting);
    private:
        Adafruit_GFX *display;
};

#ifdef INCLUDE_TEST_SCREENS

class ScreenTest : public ScreenBase {
    public:
        void setup(Adafruit_GFX *display);
        void loop();
        void finish();
        void setting_update(const char* setting, const char *new_setting);
    private:
        Adafruit_GFX *display;
};

class ScreenTestB : public ScreenBase {
    public:
        void setup(Adafruit_GFX *display);
        void loop();
        void finish();
        void setting_update(const char* setting, const char *new_setting);
    private:
        Adafruit_GFX *display;
};

class BrightnessTest: public ScreenBase {
    public:
        void setup(Adafruit_GFX *display);
        void loop();
        void finish();
        void setting_update(const char* setting, const char *new_setting);
    private:
        Adafruit_GFX *display;
};

class SensorTest: public ScreenBase {
    public:
        void setup(Adafruit_GFX *display);
        void loop();
        void finish();
        void setting_update(const char* setting, const char *new_setting);
    private:
        Adafruit_GFX *display;
};

class SDTest: public ScreenBase {
    public:
        void setup(Adafruit_GFX *display);
        void loop();
        void finish();
        void setting_update(const char* setting, const char *new_setting);
    private:
        Adafruit_GFX *display;
};

#endif


class ClockDigital: public ScreenBase {
    public:
        void setup(Adafruit_GFX *display);
        void loop();
        void finish();
        void setting_update(const char* setting, const char *new_setting);
    private:
        Adafruit_GFX *display;
        uint16_t rainbow_speed;
};

class GIFPlayer: public ScreenBase {
    public:
        void setup(Adafruit_GFX *display);
        void loop();
        void finish();
        void setting_update(const char* setting, const char *new_setting);
    private:
        Adafruit_GFX *display;
        const char *current_gif_path;
};



//-----------------------------------------------------
// System to store screens in array
//-----------------------------------------------------

// typedef for a function to create screen classes
typedef ScreenBase* (*ScreenCreator)();

// template function to create screen classes
template <typename T>
static ScreenBase* make() { return new T{}; }

// struct to store info about what settings each screen has
struct ScreenSettings {
    const char *name;               // display name and id of the setting
    const char *default_value;      // the default value to provide
};

// struct containing info about each screen
struct ScreenInfo {
    const char *name;                       // the display name of the screen
    ScreenCreator creator;                  // creator function to create an instance of the screen's class
    std::vector<ScreenSettings> settings;   // what settings the screen has
    bool clear_buffer;                      // whether to clear the framebuffer before each frame
};

// static ScreenCreator adfewsghtrkuy[] = {make<ScreenTest>};

// array containing all registered screens
extern std::vector<ScreenInfo> screens;




//-----------------------------------------------------
// Screen init and current screen reference
//-----------------------------------------------------

extern ScreenBase *current_screen;  // reference to object for the currently loaded screen
extern int current_screen_id;       // id of the current screen's class creator in the `screens` vector
static int next_screen_id = -1;     // internal flag + id for which screen to switch to

void setup_screens();
bool switch_screen(int new_screen_id);
void update_screen(Adafruit_GFX *display);
const char *current_screen_name();


#endif /* CLOCK_SCREENS_H */