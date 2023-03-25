#ifndef CLOCK_PINS_H
#define CLOCK_PINS_H


// whether to include test screens
// #define INCLUDE_TEST_SCREENS

// which filesystem to use
#define FILESYSTEM SD
// #define FILESYSTEM SPIFFS

// I2C pins
#define SDA_PIN 12
#define SCL_PIN 11

// SPI pins
#define SPI_MISO 10
#define SPI_MOSI 9
#define SPI_SCK 46
#define SPI_SD_CS 17


// LED matrix pins

#define R1_PIN 4
#define G1_PIN 5
#define B1_PIN 6
#define R2_PIN 7
#define G2_PIN 15
#define B2_PIN 16
#define A_PIN  18
#define B_PIN  8
#define C_PIN  3
#define D_PIN  42
#define E_PIN  39 // required for 1/32 scan panels, like 64x64. Any available pin would do, i.e. IO32
#define LAT_PIN 40
#define OE_PIN  2
#define CLK_PIN 41

#define PANEL_RES_X 64     // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 64     // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1      // Total number of panels chained one to another

#endif /* CLOCK_PINS_H */