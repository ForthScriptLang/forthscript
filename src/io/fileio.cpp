#include <fstream>
#include <io/encodings.hpp>
#include <io/fileio.hpp>
#include <sstream>

std::optional<std::u32string> readFile(const char *filename) {
    std::ifstream input(filename);
    if (!input.is_open()) {
        return std::optional<std::u32string>();
    }
    std::stringstream buffer;
    buffer << input.rdbuf();

    return toUTF32(buffer.str());
}
