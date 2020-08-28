#include <fstream>
#include <io/fileio.hpp>
#include <sstream>

std::u32string toUTF32(const std::string &s);
std::string fromUTF32(const std::u32string &s);

std::optional<std::u32string> readFile(const char *filename) {
    std::ifstream input(filename);
    if (!input.is_open()) {
        return std::optional<std::u32string>();
    }
    std::stringstream buffer;
    buffer << input.rdbuf();

    return toUTF32(buffer.str());
}

bool writeFile(const std::u32string &name, const std::u32string &contents) {
    std::ofstream output(fromUTF32(name));
    if (!output.is_open()) {
        return false;
    }
    output << fromUTF32(contents);
    if (output.bad()) {
        return false;
    }
    return true;
}