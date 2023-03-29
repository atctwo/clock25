#include "pins.h"
#include "display.h"
#include "log.h"
#include "fonts/varela_round_6.h"
#include "fonts/varela_round_7.h"

#define LOG_TAG "display"

MatrixPanel_I2S_DMA *display = nullptr;

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
        display->setBrightness8(90); //0-255

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