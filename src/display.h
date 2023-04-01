#ifndef CLOCK_DISPLAY_H
#define CLOCK_DISPLAY_H

/*
 * objects and functions to drive the LED matrix
 * using https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-DMA 
*/

#include <functional>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// The LED matrix display object
extern MatrixPanel_I2S_DMA *display;
extern uint8_t display_brightness;

/**
 * Function to create and configure the display.
 * This should be called before `display` is used.
*/
void setup_display();

/**
 * Set the brightness of the display
 * @param new_brightness the brightness of the display, from 0 - 255
*/
void set_display_brightness(uint8_t new_brightness);

/**
 * Get the current display brightness
 * @return the brightness of the display, from 0 - 255
*/
uint8_t get_display_brightness();

/**
 * @brief Fades the display in or out
 * 
 * Changes the brightness from the current level to a target level over a specified time.  This function
 * is asynchronous, so the actual brightness updates are done by `update_fade()`, which needs to be called
 * each frame.  This allows states to update while fading is in progress (so they don't just freeze).
 * 
 * The target brightness is specified as an integer from 0 (display off) to 255 (full brightness).  The
 * fade direction (in or out) is automatically determined.  If target is omitted, it will default to the
 * current "regular" brightness (which isn't affected by this function).
 * 
 * You can specify a callback function, which will be called when the fade is complete.  This can be used
 * to fade out, then do something, and then fade back in, by calling the something code and fade in in the
 * callback of the call to fade out.
 * 
 * @param target the target brightness, from 0 to 255
 * @param callback a function to call when the fade is complete
 * @param fade_time the time (in milliseconds) that the fade animation should last
 * @param fade_out_in whether the call to this function is part of a fade out - fade in cycle
*/
void fade_display(uint8_t target=display_brightness, std::function<void()> callback=nullptr, uint16_t fade_time=40, bool fade_out_in=false);

/**
 * If there is a fade in progress, this function changes the screen brightness when needed.
*/
void update_fade();

#endif /* CLOCK_DISPLAY_H */