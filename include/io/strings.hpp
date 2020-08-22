#pragma once

#include <core/heap.hpp>
#include <string>
#include <string_view>

inline int64_t parseIntFrom(const std::u32string_view &view) {
    bool sign = false;
    int64_t result = 0;
    for (char32_t c : view) {
        if (c == U'-') {
            sign = true;
        } else {
            result = result * 10 + (int64_t)(c - U'0');
        }
    }
    if (sign) {
        result *= -1;
    }
    return result;
}

String *convertFromBackslashed(const std::u32string_view &view, Heap &h);
std::u32string convertToBackslashed(const std::u32string &str);