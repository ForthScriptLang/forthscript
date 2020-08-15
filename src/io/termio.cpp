#include <codecvt>
#include <io/termio.hpp>
#include <iostream>
#include <locale>

std::string fromUTF32(const std::u32string &s);
std::u32string toUTF32(const std::string &s);

std::u32string readLine() {
    std::string result;
    std::getline(std::cin, result);
    return toUTF32(result);
}

void print(const std::u32string &str) { std::cout << fromUTF32(str); }