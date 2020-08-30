#pragma once

#include <string>
#include <string_view>
#include <vector>

enum class LexemeType {
    OpenSquareBrace,
    CloseSquareBrace,
    OpenCurlyBrace,
    CloseCurlyBrace,
    OpenCircleBrace,
    CloseCircleBrace,
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

inline bool isValidIdentStart(char32_t val) {
    return val == U'_' || (U'a' <= val && val <= U'z') ||
           (U'A' <= val && val <= U'Z');
}

inline bool isNumericChar(char32_t val) { return (U'0' <= val && val <= U'9'); }

inline bool isValidIdentChar(char32_t val) {
    return isValidIdentStart(val) || isNumericChar(val);
}

inline bool isWhitespace(char32_t val) {
    return val == U' ' || val == U'\t' || val == U'\n' || val == U'\r';
}

LexResult lex(const std::u32string& str);