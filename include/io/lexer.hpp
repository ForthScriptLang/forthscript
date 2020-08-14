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
    WordAssignment,
    WordDeclare,
    String
};

struct Lexeme {
    LexemeType type;
    std::u32string_view val;
    size_t pos;
};

struct LexResult {
    bool error;
    int64_t errorPos;
    std::u32string errorDescription;
    std::vector<Lexeme> lexems;
};

LexResult lex(const std::u32string& str);