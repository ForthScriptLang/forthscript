#include <io/strings.hpp>

String *convertFromBackslashed(const std::u32string_view &view, Heap &h) {
    std::u32string resultStr;
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
                    resultStr.push_back(U'\n');
                    break;
                case U'b':
                    resultStr.push_back(U'\b');
                    break;
                case U'r':
                    resultStr.push_back(U'\r');
                    break;
                case U'a':
                    resultStr.push_back(U'\a');
                    break;
                case U't':
                    resultStr.push_back(U'\t');
                    break;
                case U'\"':
                    resultStr.push_back(U'\"');
                    break;
                case U'\'':
                    resultStr.push_back(U'\'');
                    break;
                default:
                    return nullptr;
            }
        } else {
            if (view[i] == U'\'') {
                return nullptr;
            }
            resultStr.push_back(view[i]);
        }
    }
    String *result = h.makeStringObject(resultStr);
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