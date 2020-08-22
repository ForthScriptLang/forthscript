#pragma once

#include <core/interpreter.hpp>
#include <core/values.hpp>
#include <string>

struct ParseResult {
    enum class Status { Success, LexerError, ParserError } status;
    inline bool isError() { return status != Status::Success; }
    std::u32string description;
    size_t errorPos;
    Array *code;
};

ParseResult parse(const std::u32string &str, Interpreter &h);