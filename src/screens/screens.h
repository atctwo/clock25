#ifndef CLOCK_SCREENS_H
#define CLOCK_SCREENS_H

/*
hmm
*/

#include <memory>
#include <vector>
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
    private:
        Adafruit_GFX *display;
};

#ifdef INCLUDE_TEST_SCREENS

class ScreenTest : public ScreenBase {
    public:
        void setup(Adafruit_GFX *display);
        void loop();
        void finish();
    private:
        Adafruit_GFX *display;
};

class ScreenTestB : public ScreenBase {
    public:
        void setup(Adafruit_GFX *display);
        void loop();
        void finish();
    private:
        Adafruit_GFX *display;
};

class BrightnessTest: public ScreenBase {
    public:
        void setup(Adafruit_GFX *display);
        void loop();
        void finish();
    private:
        Adafruit_GFX *display;
};

class SensorTest: public ScreenBase {
    public:
        void setup(Adafruit_GFX *display);
        void loop();
        void finish();
    private:
        Adafruit_GFX *display;
};

class SDTest: public ScreenBase {
    public:
        void setup(Adafruit_GFX *display);
        void loop();
        void finish();
    private:
        Adafruit_GFX *display;
};

#endif


class ClockDigital: public ScreenBase {
    public:
        void setup(Adafruit_GFX *display);
        void loop();
        void finish();
    private:
        Adafruit_GFX *display;
};

class GIFPlayer: public ScreenBase {
    public:
        void setup(Adafruit_GFX *display);
        void loop();
        void finish();
    private:
        Adafruit_GFX *display;
};



//-----------------------------------------------------
// System to store screens in array
//-----------------------------------------------------

typedef ScreenBase* (*ScreenCreator)();

template <typename T>
static ScreenBase* make() { return new T{}; }

// struct containing info about each screen
struct ScreenInfo {
    const char *name;               // the display name of the screen
    ScreenCreator creator;          // creator function to create an instance of the screen's class
    bool clear_buffer;              // whether to clear the framebuffer before each frame
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

void switch_screen(int new_screen_id);
void update_screen(Adafruit_GFX *display);


#endif /* CLOCK_SCREENS_H */