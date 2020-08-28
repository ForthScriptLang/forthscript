#pragma once

#include <optional>
#include <string>

std::optional<std::u32string> readFile(const char *filename);
bool writeFile(const std::u32string &name, const std::u32string &contents);