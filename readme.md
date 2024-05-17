# clock25
A funky digital clock made out of an ESP32 and an RGB LED matrix panel!  This was created as a one-off personal project, but all the code and design files are included so I hope you find it useful or interesting!

<img src="extras/images/main.png" width=500 alt="Picture of the front of the clock">

## Features
- tells the time in big rainbow digits
- gets time from the internet using NTP, and has an onboard RTC for keeping time without power
- the time can also be configured using physical onboard buttons 
- supports multiple "screens" which are basically apps
- the system and each screen can be configured with a web interface
    - available at a custom URL using mDNS: [clock25.local](http://clock25.local)
- the configuration is stored on an SD card so it can be edited manually without internet 
- adaptive brightness using ambient light sensor

Currently implemented screens are
- **Digital Clock Face**: also shows current temperature and humidity if an SHTC3 is connected
- **Weather Forecast**: gets weather from the internet
- **GIF Player**:  plays GIFs from SD card

# Implementation
## Hardware
### HUB75 RGB LED matrix
The clock has a big HUB75-style RGB LED matrix panel.  I used one which has 64x64 pixels.

### Custom PCB
<img src="extras/images/pcb.jpg" width=500 alt="Picture of the PCB">

The project is driven by a custom PCB.  It holds:
- an ESP32-S3
- a PCF8523 RTC and a CR2032 coin cell battery holder
- a microSD card slot for storing the config file (and GIFs)
    - [an SHTC3 temperature sensor](https://www.adafruit.com/product/4636)
    - [a VEML7700 ambient light sensor](https://www.adafruit.com/product/4162)
    - [a APDS9960 gesture sensor](https://www.adafruit.com/product/3595)
- a 2x10 0.1" header for connecting to the HUB75 panel
- a USB-C connector for power (5V, 3A)
- onboard buttons for setting the time or changing screen

It's made with KiCad and located in the [pcb](./pcb/) folder.

### 3D Printed Frame
<img src="extras/images/back.png" width=500 alt="Picture of the back of the clock">

The project includes a 3D printed frame which holds the matrix and the PCB.  Although the assembled version uses a JLCPCB box superglued onto a minimal frame because I didn't have enough filament to print the whole frame.  Note that the frame is designed to fit the obscure shape of the panel I have, if you want to make a clock25 you might need to modify the OpenSCAD file to fit the dimensions of your panel.

It's written in OpenSCAD using the [MCAD](https://github.com/openscad/MCAD) library, and is located in the [frame](./frame/) folder

## Software
### Configuration System
When screens are registered at compile time, they specify a series of key-value configuration items.  For example, the Digital Clock Face has a setting called "Rainbow Speed" which determines the speed of the rainbow text animation.  Values for each setting are stored in memory.  If an SD card is mounted, when settings are overwritten they are stored in a JSON file on the SD card.  When the clock boots it will load saved values from the SD card.  Each setting can be modified from the web interface without rebooting the clock.

A sample config JSON file is located at [extras/sd_layout/settings.json](extras/sd_layout/settings.json).

### Recommended SD card layout
Different parts of the system depend on some files being at specific locations on the SD card:
- Files for the Web Interface are hosted from /web/
- Weather Forecast expects weather icons to be located at /weather/
- GIF Player loads gifs from /gifs/

As such, there is a recommended layout for the SD card:

```plain
.
├── gifs
│   ├── example.gif
├── settings.json
├── weather
│   ├── 01d.bmp
│   ├── 01n.bmp
│   ├── ...
│   └── 50n.bmp
└── web
    ├── ball.css
    ├── index.css
    ├── index.html
    └── index.js
```

This layout is provided at [extras/sd_layout](extras/sd_layout/), so if you're setting up the SD card you can just copy the contents of this folder to the root of the SD.

### Web Interface
[screenshot of the web interface]

When connected to a network, the clock provides a web interface for configuring the system and each screen.  It's hosted on port 80.  The clock announces a custom URL using mDNS: [clock25.local](http://clock25.local).

### Building
The project is built using PlatformIO, so make sure this is installed before building.  The project is designed for the ESP32-S3 using the Arduino framework, although it uses a lot of ESP32-specific function calls.

To build the code, open the project in VSCode.  PlatformIO should automatically initialise and will automatically download any required dependancies and libraries.  Go to the PlatformIO tab and click on Upload and Monitor.

# Credits
## Code Libraries
- [AnimatedGIF](https://github.com/bitbank2/AnimatedGIF) library for loading and displaying animated GIFs
- [AdafruitGFX](https://github.com/adafruit/Adafruit-GFX-Library) - graphics primitives used for the display
- [ESP32 HUB75 Matrix Panel DMA](https://github.com/mrcodetastic/ESP32-HUB75-MatrixPanel-DMA) for displaying graphics on the HUB75 matrix using DMA
- [Adafruit VEML7700 Library](https://github.com/adafruit/Adafruit_VEML7700) for the VEML7700 ambient light sensor
- [Adafruit APDS9960 Library](https://github.com/adafruit/Adafruit_APDS9960) for the APDS9960 gesture sensor
- [Adafruit SHTC3 Library](https://github.com/adafruit/Adafruit_SHTC3) for the SHTC3 temperature and humidity sensor
- [RTClib](https://github.com/adafruit/RTClib) for interfacing with the PCF8523 real time clock
- [Adafruit BusIO](https://github.com/adafruit/Adafruit_BusIO) I2C abstractions for Adafruit's sensor libraries
- [Adafruit Unified Sensor Library](https://github.com/adafruit/Adafruit_Sensor) common defines for Adafruit's sensor libraries
- [AdafruitJSON](https://github.com/bblanchon/ArduinoJson) library for serialisation/deserialisation of JSON documents
- Image loading using [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h) by nothings
- OpenSCAD [MCAD](https://github.com/openscad/MCAD) library is used in the frame

## Assets
- Weather icons by [Royyan Wijaya](https://www.flaticon.com/packs/weather-1050)
- Fonts used are [Varela Round](https://fonts.google.com/specimen/Varela+Round) and [Open Sans](https://fonts.google.com/specimen/Open+Sans?query=open+sans)