#pragma once

#include <core/heap.hpp>
#include <string>
#include <string_view>

inline int64_t parseIntFrom(const std::u32string_view &view) {
    int64_t result = 0;
    for (char32_t c : view) {
        result = result * 10 + (int64_t)(c - U'0');
    }
    return result;
}

String *convertFromBackslashed(const std::u32string_view &view, Heap &h);
std::u32string convertToBackslashed(const std::u32string &str);