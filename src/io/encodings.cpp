#include <ConvertUTF.h>

#include <codecvt>
#include <locale>

using namespace linenoise_ng;

#ifdef __WIN32
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

bool validUTF32(char32_t ch) {
    char32_t utf32buf[2] = {ch, U'\0'};
    char utf8buf[5];
    UTF32 *sourceStart = (UTF32 *)utf32buf;
    UTF32 *sourceEnd = (UTF32 *)(utf32buf + 1);
    UTF8 *targetStart = (UTF8 *)utf8buf;
    UTF8 *targetEnd = (UTF8 *)(utf8buf + 5);

    ConversionResult st =
        ConvertUTF32toUTF8((const UTF32 **)&sourceStart, sourceEnd,
                           &targetStart, targetEnd, strictConversion);
    return st == conversionOK;
}