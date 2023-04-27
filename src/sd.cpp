#include "sd.h"
#include "SPI.h"
#include "pins.h"
#include "log.h"

#define LOG_TAG "sd"

// SPIClass spi = SPIClass(HSPI);
bool is_sd_setup = false;

bool setup_sd()
{
    logi(LOG_TAG, "setting up sd card");

    if (is_sd_setup) SD.end();

    // logi(LOG_TAG, "starting spi");
    // spi.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_SD_CS);

    // logi(LOG_TAG, "starting sd card");

    // // SD.begin(SPI_SD_CS, spi, 4000000);
    // // while(!SD.begin(SPI_SD_CS, spi, 1000000)) {}

    // logi(LOG_TAG, "sd setup!");


    is_sd_setup = SD.begin(SPI_SD_CS, SPI, 1000000);
    return is_sd_setup;
}

bool sd_available()
{
    return is_sd_setup;
}