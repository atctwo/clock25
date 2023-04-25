# clock25

a funky digital clock made out of an esp32 and an rgb matrix!  this is a work in progress.

## features

- gets time from the internet using NTP
- configuation interface available using a web browser
- internet connectivity is optional!  it can also be configured by editing the config file on an SD card
- adaptive brightness using ambient light sensor
- shows current temperature and humidity
- weather forecast (using internet)
- plays GIFs from and SD card

## building

the code is made using platformio, and is heavily targeted for the esp32.  it's also designed with specific hardware in mind, but the in-code interface tries to be hardware-agnostic so that hardware changes are easier to make.

## credits
- Image loading using [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h) by nothings
- Weather icons by [Royyan Wijaya](https://www.flaticon.com/packs/weather-1050)