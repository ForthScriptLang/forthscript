#pragma once

#include <core/heap.hpp>
#include <string>
#include <string_view>

inline int64_t parseIntFrom(std::u32string_view view) {
    int64_t result = 0;
    for (char32_t c : view) {
        result = result * 10 + (int64_t)(c - U'0');
    }
    return result;
}

inline String *convertFromBackslashed(std::u32string_view view, Heap &h) {
    std::u32string_view dummy;
    String *result = h.makeStringObject(dummy);
    size_t len = view.size();
    for (size_t i = 0; i < len; ++i) {
        if (result->str[i] == U'\\') {
            if (i == len - 1) {
                return nullptr;
            }
            ++i;
            char sym = result->str[i];
            switch (sym) {
                case U'n':
                    result->str.push_back(U'\n');
                    break;
                case U'b':
                    result->str.push_back(U'\b');
                    break;
                case U'r':
                    result->str.push_back(U'\r');
                    break;
                case U'a':
                    result->str.push_back(U'\a');
                    break;
                case U't':
                    result->str.push_back(U'\t');
                    break;
                case U'\"':
                    result->str.push_back(U'\"');
                    break;
                case U'\'':
                    result->str.push_back(U'\'');
                    break;
                default:
                    return nullptr;
            }
        } else {
            if (result->str[i] == U'\'') {
                return nullptr;
            }
            result->str.push_back(result->str[i]);
        }
    }
    return result;
}