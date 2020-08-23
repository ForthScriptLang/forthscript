#include <fstream>
#include <io/fileio.hpp>
#include <sstream>

std::u32string toUTF32(const std::string &s);

std::optional<std::u32string> readFile(const char *filename) {
    std::ifstream input(filename);
    if (!input.is_open()) {
        return std::optional<std::u32string>();
    }
    std::stringstream buffer;
    buffer << input.rdbuf();

    return toUTF32(buffer.str());
}
