#include "pins.h"
#include "display.h"
#include "log.h"

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

        logi(LOG_TAG, "initialised display! :)");

    }
}