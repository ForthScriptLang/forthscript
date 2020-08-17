#pragma once

#include <core/interpreter.hpp>
#include <string>

void initREPL(Interpreter& interpreter);
std::u32string readLine(const std::u32string& prompt,
                        bool disableAutocomplete = false);
void print(const std::u32string& str);