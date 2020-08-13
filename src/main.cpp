#include <codecvt>
#include <io/parser.hpp>
#include <io/termio.hpp>
#include <iostream>
#include <locale>

int main() {
    while (true) {
        print(U">>> ");
        std::u32string line = readLine();
        print(line);
        print(U"\n");
    }
}