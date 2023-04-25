#include "image.h"
#include "sd.h"
#include "utility.h"
#include "log.h"

#define STB_IMAGE_IMPLEMENTATION
// #define STBI_MALLOC(sz)           heap_caps_malloc(sz, MALLOC_CAP_SPIRAM)
// #define STBI_REALLOC(p,newsz)     heap_caps_realloc(p,newsz, MALLOC_CAP_SPIRAM)
// #define STBI_FREE(p)              free(p)
#include "stb_image.h"

#define LOG_TAG "image"

// functions for stb_image
int img_read(void *user,  char *data, int size)
{
    fs::File *f = static_cast<fs::File*>(user);
    int bytes_read = f->readBytes(data, size);
    return bytes_read;
}

void img_skip(void *user, int n)
{
    fs::File *f = static_cast<fs::File*>(user);
    f->seek(n);
}

int img_eof(void *user)
{
    fs::File *f = static_cast<fs::File*>(user);
    uint32_t help = f->available();
    if (help == 0) return 1;
    return 0;
}

stbi_io_callbacks stbi_callbacks = {
    img_read,
    img_skip,
    img_eof
};

bool draw_image(const char *filename, Adafruit_GFX *display, uint16_t x, uint16_t y, FS fs)
{
    logi(LOG_TAG, "Drawing image to screen: %s", filename);
    bool result = false;

    if (sd_available())
    {
        // open file
        fs::File file = fs.open(filename);
        if (file)
        {
            // load image
            int w, h, n;
            unsigned char *data = stbi_load_from_callbacks(&stbi_callbacks, &file, &w, &h, &n, 3);

            if (data)
            {
                // logi(LOG_TAG, "Loaded image - it's %d x %d - size of data = %d", w, h, sizeof(data));

                // free image memory
                stbi_image_free(data);

                // draw image
                uint16_t px = 0;
                uint16_t py = 0;
                uint32_t size = w * h * 3;

                for (uint32_t i = 0; i < size; i += 3)
                {
                    // convert rgb888 to rgb565
                    uint16_t colour = rgb888torgb565(data[i], data[i+1], data[i+2]);

                    // draw pixel
                    display->drawPixel(px+x, py+y, colour);

                    // update position
                    px++;
                    if (px >= w) {
                        px = 0;
                        py++;
                    }
                }

                // set result
                result = true;
            }
            else
            {
                logw(LOG_TAG, "Error loading image: %s", stbi_failure_reason());
            }

            // close file
            file.close();

        }
        else logw(LOG_TAG, "Error loading image: can't open file");
    }
    else logw(LOG_TAG, "Error loading image: SD card not mounted");

    return result;
}