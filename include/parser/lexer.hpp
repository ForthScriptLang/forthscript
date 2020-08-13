#pragma once

#include <string>
#include <string_view>
#include <vector>

enum class LexemeType {
    OpenSquareBrace,
    CloseSquareBrace,
    OpenCurlyBrace,
    CloseCurlyBrace,
    Number,
    Word,
    String
};

struct Lexeme {
    LexemeType type;
    std::u32string_view val;
};

struct Lexems {
    bool error;
    int64_t errorPos;
    std::vector<Lexeme> lexems;
};

Lexems lex(const std::u32string& str);