#include "pins.h"
#ifdef INCLUDE_TEST_SCREENS

#include "../screens.h"
#include "sd.h"
#include "log.h"

#define LOG_TAG "screen:sd_test"



// from https://randomnerdtutorials.com/esp32-microsd-card-arduino/
void listDir(Adafruit_GFX *display, fs::FS &fs, const char * dirname, uint8_t levels){

    File root = fs.open(dirname);
    if(!root){
        display->println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        display->println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){

            // display->print("  DIR : ");
            display->println(file.name());

            if(levels){
                std::string fname = "/";
                fname += file.name();
                listDir(display, fs, fname.c_str(), levels -1);
            }

        } else {
            // display->print("  FILE: ");
            display->println(file.name());
            // display->print("  SIZE: ");
            // display->println(file.size());
        }
        file = root.openNextFile();
    }

}



void SDTest::setup(Adafruit_GFX *display)
{
    logi(LOG_TAG, "SD Test!!!");
    this->display = display;
}

void SDTest::loop()
{
    this->display->setCursor(2, 2);
    this->display->setTextColor(0x067A);
    this->display->printf("hello :)\n");

    this->display->setTextColor(0xffff);

    if (sd_available()) listDir(this->display, SD, "/", 1);
    else this->display->println("SD not\navailable");
}

void SDTest::finish()
{
    logi(LOG_TAG, "Finish!!!");
}

void SDTest::setting_update(const char* setting, const char *new_setting)
{
    
}

#endif /* INCLUDE_TEST_SCREENS */