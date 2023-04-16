#include "pins.h"
#include "sd.h"
#include "settings.h"
#include "screens.h"
#include "log.h"

#include <AnimatedGIF.h>
#include <SPIFFS.h>
#include <SD.h>

#define LOG_TAG "screen:gif_player"

Adafruit_GFX *dma_display;
AnimatedGIF *gif;
File f;
int x_offset, y_offset;
bool is_gif_open = false;

char filePath[256] = { 0 };
File root, gifFile;
bool has_root_been_opened = false;
unsigned long start_tick = 0;

bool cycle_gifs = false;
// const char *default_gif = "/gifs/amogus64.gif";

const char *next_gif = nullptr;


// Draw a line of image directly on the LED Matrix
void GIFDraw(GIFDRAW *pDraw)
{
    logd(LOG_TAG, "gif draw start");

    uint8_t *s;
    uint16_t *d, *usPalette, usTemp[320];
    int x, y, iWidth;

  iWidth = pDraw->iWidth;
  if (iWidth > PANEL_RES_X)
      iWidth = PANEL_RES_X;

    usPalette = pDraw->pPalette;
    y = pDraw->iY + pDraw->y; // current line
    
    s = pDraw->pPixels;
    if (pDraw->ucDisposalMethod == 2) // restore to background color
    {
      for (x=0; x<iWidth; x++)
      {
        if (s[x] == pDraw->ucTransparent)
           s[x] = pDraw->ucBackground;
      }
      pDraw->ucHasTransparency = 0;
    }
    // Apply the new pixels to the main image
    if (pDraw->ucHasTransparency) // if transparency used
    {
      uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
      int x, iCount;
      pEnd = s + pDraw->iWidth;
      x = 0;
      iCount = 0; // count non-transparent pixels
      while(x < pDraw->iWidth)
      {
        c = ucTransparent-1;
        d = usTemp;
        while (c != ucTransparent && s < pEnd)
        {
          c = *s++;
          if (c == ucTransparent) // done, stop
          {
            s--; // back up to treat it like transparent
          }
          else // opaque
          {
             *d++ = usPalette[c];
             iCount++;
          }
        } // while looking for opaque pixels
        if (iCount) // any opaque pixels?
        {
          for(int xOffset = 0; xOffset < iCount; xOffset++ ){
            dma_display->drawPixel(x + xOffset, y, usTemp[xOffset]); // 565 Color Format
          }
          x += iCount;
          iCount = 0;
        }
        // no, look for a run of transparent pixels
        c = ucTransparent;
        while (c == ucTransparent && s < pEnd)
        {
          c = *s++;
          if (c == ucTransparent)
             iCount++;
          else
             s--; 
        }
        if (iCount)
        {
          x += iCount; // skip these
          iCount = 0;
        }
      }
    }
    else // does not have transparency
    {
      s = pDraw->pPixels;
      // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
      for (x=0; x<pDraw->iWidth; x++)
      {
        dma_display->drawPixel(x, y, usPalette[*s++]); // color 565
      }
    }

    logd(LOG_TAG, "gif draw end");
} /* GIFDraw() */


void * GIFOpenFile(const char *fname, int32_t *pSize)
{
    logd(LOG_TAG, "gif open start");
    Serial.print("Playing gif: ");
    Serial.println(fname);
    f = FILESYSTEM.open(fname);
    if (f)
    {
        *pSize = f.size();
        return (void *)&f;
    }
    logd(LOG_TAG, "gif open end");
    return NULL;
} /* GIFOpenFile() */

void GIFCloseFile(void *pHandle)
{
  File *f = static_cast<File *>(pHandle);
  if (f != NULL)
     f->close();
} /* GIFCloseFile() */

int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
    logd(LOG_TAG, "gif read start");
    int32_t iBytesRead;
    iBytesRead = iLen;
    File *f = static_cast<File *>(pFile->fHandle);
    // Note: If you read a file all the way to the last byte, seek() stops working
    if ((pFile->iSize - pFile->iPos) < iLen)
       iBytesRead = pFile->iSize - pFile->iPos - 1; // <-- ugly work-around
    if (iBytesRead <= 0)
       return 0;
    iBytesRead = (int32_t)f->read(pBuf, iBytesRead);
    pFile->iPos = f->position();
    logd(LOG_TAG, "gif read end");
    return iBytesRead;
} /* GIFReadFile() */

int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition)
{ 
    logd(LOG_TAG, "gif seek start");
    int i = micros();
    File *f = static_cast<File *>(pFile->fHandle);
    f->seek(iPosition);
    pFile->iPos = (int32_t)f->position();
    i = micros() - i;
    //  Serial.printf("Seek time = %d us\n", i);
    logd(LOG_TAG, "gif seek end");
    return pFile->iPos;
} /* GIFSeekFile() */



void load_gif_from_file(std::string default_gif)
{
    logi(LOG_TAG, "eeeeeeee %x", default_gif);
    if (sd_available())
    {

        // close gif if already open
        if (is_gif_open) {

            // close gif
            gif->close();

            // delete gif
            delete gif;

            is_gif_open = false;
        }


        gif = new AnimatedGIF();
        gif->begin(LITTLE_ENDIAN_PIXELS);

        start_tick = millis();

        // char gif_path[200] = "";
        const char *gif_path = default_gif.c_str();

        if (!has_root_been_opened) {
            File root = FILESYSTEM.open("/gifs");
            has_root_been_opened = true;
        }

        File gifFile = root.openNextFile();

        if (cycle_gifs)
        {
            if (!gifFile) {
                root.close();
                root = FILESYSTEM.open("/gifs");
                gifFile = root.openNextFile();
            }

            // strcat(gif_path, "/gifs/");
            // strcat(gif_path, gifFile.name());
        }
        else
        {
            // if cycle is disabled, copy the default gif path
            // strcpy(gif_path, default_gif);
        }

        logi(LOG_TAG, "opening gif %s", gif_path);
        if (gif->open(gif_path, GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, GIFDraw))
        {
            x_offset = (PANEL_RES_X - gif->getCanvasWidth())/2;
            if (x_offset < 0) x_offset = 0;
            y_offset = (PANEL_RES_Y - gif->getCanvasHeight())/2;
            if (y_offset < 0) y_offset = 0;

            Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif->getCanvasWidth(), gif->getCanvasHeight());
            Serial.flush();

            is_gif_open = true;
        }
        else
        {
            is_gif_open = false;
            loge(LOG_TAG, "failed to open gif %s", gif_path);
            delete gif;
        }

    }
}


void set_setting_values_to_gifs()
{
    logi(LOG_TAG, "Determining list of gif paths for setting values");

    // create vector to store paths
    std::vector<std::string> gif_paths;

    // iterate over files in directory
    File root = FILESYSTEM.open("/gifs");
    File file = root.openNextFile();
    while(file) {

        // if file isn't a directory
        if (!file.isDirectory())
        {
            // add filename to vector
            std::string filename = "/gifs/";
            filename += file.name();
            gif_paths.push_back(filename);
        }

        // check next file
        file = root.openNextFile();
    }

    // set setting values
    set_setting_values("GIF Player", "GIF File", gif_paths);
}



void GIFPlayer::setup(Adafruit_GFX *display)
{
    this->display = display;
    dma_display = display; // HACK

    // load settings
    this->current_gif_path = get_setting(nullptr, "GIF File", "/gifs/amogus64.gif").c_str();

    // load gif
    this->display->fillScreen(0);
    load_gif_from_file(this->current_gif_path);

    // determine setting values
    set_setting_values_to_gifs();
}

void GIFPlayer::loop()
{
    if (!sd_available())
    {
        display->setCursor(0, 0);

        this->display->setTextColor(0x067A, 0);
        this->display->printf("GIF Player\n");

        display->setTextColor(0xffff, 0);
        display->printf("SD not    \navailable ", gifFile.name());
    }
    else
    {

        if (is_gif_open) {

            // play the next gif frame
            bool not_finished = gif->playFrame(true, NULL);

            // go back to the start of the gif
            if (!not_finished) {

                // reset the gif (back to the start)
                gif->reset();

                // check if there is a new gif to load
                if (next_gif != nullptr) {

                    // load next gif
                    this->display->fillScreen(0);
                    this->current_gif_path = next_gif;
                    load_gif_from_file(next_gif);

                }
            }
        }
        else {

            display->setCursor(0, 0);

            this->display->setTextColor(0x067A, 0);
            this->display->printf("GIF Player\n");

            display->setTextColor(0xffff, 0);
            // logi(LOG_TAG, "eeeeeeee %s", this->current_gif_path);
            display->printf("Failed to\nload GIF\n\n%s", this->current_gif_path);

        }

    }
}

void GIFPlayer::finish()
{
    if (is_gif_open)
    {
        // close gif
        gif->close();

        // destroy gif
        delete gif;

        is_gif_open = false;
    }
}

void GIFPlayer::setting_update(const char* setting, const char *new_setting)
{
    // if setting is gif file
    if (strcmp(setting, "GIF File") == 0) {

        // // if there isn't a gif already open
        // if (!is_gif_open) {
        //     // load the gif
        //     this->current_gif_path = new_setting;
        //     load_gif_from_file(new_setting);
        // } 
        // // otherwise, save the gif file path to be loaded later
        // else next_gif = new_setting;

        this->display->fillScreen(0);
        this->current_gif_path = new_setting;
        load_gif_from_file(new_setting);
    }
}