#include "pins.h"
#include "display.h"
#include "log.h"
#include "fonts/varela_round_6.h"
#include "fonts/varela_round_7.h"

#define LOG_TAG "display"

MatrixPanel_I2S_DMA *display = nullptr;
uint8_t display_brightness = 255;

uint16_t step_time = 0;
uint8_t step_size = 1;
uint32_t last_step_time = 0;
int16_t current_brightness = 90;
uint16_t target_brightness = 0;
std::function<void()> fade_callback = nullptr;
int fade_active = 0;
bool fade_out_in_cycle = false;

void setup_display()
{
    if (!display) // if display isn't already setup
    {
        logi(LOG_TAG, "initalising display");

        // Module configuration
        HUB75_I2S_CFG::i2s_pins panel_pins = {R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN};
        HUB75_I2S_CFG mxconfig(
            PANEL_RES_X,   // module width
            PANEL_RES_Y,   // module height
            PANEL_CHAIN,   // Chain length
            panel_pins     // pins
        );

        //mxconfig.gpio.e = 18;
        //mxconfig.clkphase = false;
        //mxconfig.driver = HUB75_I2S_CFG::FM6126A;

        // Display Setup
        display = new MatrixPanel_I2S_DMA(mxconfig);
        display->begin();
        // set_display_brightness(90); //0-255

        display_brightness = 90;
        current_brightness = 90;
        display->setBrightness(90);

        // show splash screen
        int16_t x, y;
        uint16_t w, h;
        display->setFont(&VarelaRound_Regular6pt7b);
        display->getTextBounds("clock25", 0, 0, &x, &y, &w, &h);
        display->setCursor((PANEL_RES_X/2) - (w/2), (PANEL_RES_Y/2) + (h/2));
        display->print("clock25");

        logi(LOG_TAG, "initialised display! :)");

    }
}

void set_display_brightness(uint8_t new_brightness) 
{ 
    if (!fade_out_in_cycle)
    {
        // logi(LOG_TAG, "updating brightness to %d", new_brightness);
        display_brightness = new_brightness; 

        fade_display(display_brightness, nullptr, 10);

        // current_brightness = new_brightness;
        // display->setBrightness(display_brightness);
    }
}

uint8_t get_display_brightness() { return display_brightness; }


void fade_display(uint8_t target, std::function<void()> callback, uint16_t fade_time, bool fade_out_in)
{
    // if brightness is at target, return
    if (current_brightness == target) return;

    // determine fade direction
    if (current_brightness > target) fade_active = 1; // fade out
    else                             fade_active = 2; // fade in

    // set temp brightness
    target_brightness = target;

    // set fade out in cycle - fade out part
    if (fade_out_in && fade_active == 1) fade_out_in_cycle = true;

    // calculate how long to stay on each brightness level
    // and how much to change the brightness for each level
    uint8_t brightness_range = abs(current_brightness - target);

    step_time = fade_time / brightness_range;
    step_size = brightness_range / fade_time;
    if (step_size < 1) step_size = 1;

    // store callback
    if (callback) fade_callback = callback;

    // logi(LOG_TAG, "fading screen, active = %d, current = %d, target = %d, step time = %d ms, step size = %d, range = %d", fade_active, current_brightness, target, step_time, step_size, brightness_range);
}

void update_fade()
{
    // if the display is currently fading
    if (fade_active)
    {
        // if the fade step period has elapsed
        if (millis() - last_step_time > step_time)
        {
            // change temp brightness depending on fade type
            if (fade_active == 1) current_brightness -= step_size;
            if (fade_active == 2) current_brightness += step_size;

            if (current_brightness < 0) current_brightness = 0;
            if (current_brightness > 255) current_brightness = 255;

            // set brightness
            // logi(LOG_TAG, "current=%d target=%d step_size=%d active=%d", current_brightness, target_brightness, step_size, fade_active);
            display->setBrightness(current_brightness);

            // check if fade has ended
            if ((fade_active == 1 && current_brightness <= target_brightness) || (fade_active == 2 && current_brightness >= target_brightness))
            {
                // logi(LOG_TAG, "fade finished!");

                // clear fade out in cycle flag
                if (fade_out_in_cycle && fade_active == 1) fade_out_in_cycle = false;

                // reset fade variables
                step_time = 0;
                fade_active = 0;

                // call callback
                if (fade_callback) fade_callback();
                // else logi(LOG_TAG, "no fade callback");

                // reset callback
                fade_callback = nullptr;
            }
            
            // update timer
            last_step_time = millis();
        }

    }
}