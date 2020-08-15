#include <io/strings.hpp>

String *convertFromBackslashed(const std::u32string_view &view, Heap &h) {
    std::u32string_view dummy;
    String *result = h.makeStringObject(dummy);
    size_t len = view.size();
    for (size_t i = 0; i < len; ++i) {
        if (view[i] == U'\\') {
            if (i == len - 1) {
                return nullptr;
            }
            ++i;
            char sym = view[i];
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
            if (view[i] == U'\'') {
                return nullptr;
            }
            result->str.push_back(view[i]);
        }
    }
    return result;
}

std::u32string convertToBackslashed(const std::u32string &str) {
    std::u32string result;
    size_t len = str.size();
    for (size_t i = 0; i < len; ++i) {
        if (str[i] == '\n') {
            result.append(U"\\n");
        } else if (str[i] == '\b') {
            result.append(U"\\b");
        } else if (str[i] == '\r') {
            result.append(U"\\r");
        } else if (str[i] == '\a') {
            result.append(U"\\a");
        } else if (str[i] == '\t') {
            result.append(U"\\t");
        } else if (str[i] == '\'') {
            result.append(U"\\\'");
        } else if (str[i] == '\"') {
            result.append(U"\\\"");
        } else {
            result.push_back(str[i]);
        }
    }
    return result;
}