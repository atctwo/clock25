
#include <utility.h>

// from https://stackoverflow.com/a/4654718
bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

// modified version of arduino map that uses templates
// https://www.arduino.cc/reference/en/language/functions/math/map/
template<typename T>
T map2(T x, T in_min, T in_max, T out_min, T out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}