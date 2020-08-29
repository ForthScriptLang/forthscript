#include <linenoise.h>

#include <algorithm>
#include <codecvt>
#include <io/termio.hpp>
#include <iostream>
#include <locale>
#include <unordered_set>

std::string fromUTF32(const std::u32string &s);
std::u32string toUTF32(const std::string &s);

std::u32string readLine(const std::u32string &prompt,
                        [[maybe_unused]] bool disableAutocomplete) {
    std::string promptUtf8 = fromUTF32(prompt);
    std::string result(linenoise(promptUtf8.c_str()));
    linenoiseHistoryAdd(result.c_str());
    return toUTF32(result);
}

void print(const std::u32string &str) {
    std::cout << fromUTF32(str);
    std::flush(std::cout);
}

void initREPL([[maybe_unused]] Interpreter &interpreter) {
    linenoiseInstallWindowChangeHandler();
}