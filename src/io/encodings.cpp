#include <codecvt>
#include <io/encodings.hpp>
#include <locale>

#ifdef __WIN32
// windows, just shut up
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#endif

std::string fromUTF32(const std::u32string &s) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    return conv.to_bytes(s);
}

std::u32string toUTF32(const std::string &s) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    return conv.from_bytes(s);
}