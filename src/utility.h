#ifndef CLOCK_UTILITY_H
#define CLOCK_UTILITY_H

#include <string>

bool is_number(const std::string& s);

template<typename T>
T map2(T x, T in_min, T in_max, T out_min, T out_max);

#endif /* CLOCK_UTILITY_H */