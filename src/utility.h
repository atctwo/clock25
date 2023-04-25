#ifndef CLOCK_UTILITY_H
#define CLOCK_UTILITY_H

#include <string>
#include <vector>
#include <array>
#include <math.h>

// from https://stackoverflow.com/a/4654718
bool is_number(const std::string& s);

// modified version of arduino map that uses templates
// https://www.arduino.cc/reference/en/language/functions/math/map/
template<typename T>
T map2(T x, T in_min, T in_max, T out_min, T out_max) {
  return fmax(out_min, fmin(out_max, (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min));
}

// convert a colour in HSV space to RGB
// from https://www.cs.rit.edu/~ncs/color/t_convert.html#RGB%20to%20HSV%20&%20HSV%20to%20RGB
void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v );

// RGB888 to RGB565
// adapted from https://afterthoughtsoftware.com/posts/convert-rgb888-to-rgb565
uint16_t rgb888torgb565(uint8_t red, uint8_t green, uint8_t blue);

//from http://www.andrewnoske.com/wiki/Code_-_heatmaps_and_color_gradients
void getHeatMapColor(float value, float *red, float *green, float *blue, std::vector<std::array<float, 3>> heatmap);

#endif /* CLOCK_UTILITY_H */