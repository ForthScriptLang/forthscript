#include <io/lexer.hpp>

enum class LexerState { Undefined, String, Identifier, Number, Comment };

bool isValidIdentStart(char32_t val) {
    return val == U'_' || (U'a' <= val && val <= U'z') ||
           (U'A' <= val && val <= U'Z');
}

bool isNumericChar(char32_t val) { return (U'0' <= val && val <= U'9'); }

bool isValidIdentChar(char32_t val) {
    return isValidIdentStart(val) || isNumericChar(val);
}

bool isWhitespace(char32_t val) {
    return val == U' ' || val == U'\t' || val == U'\n' || val == U'\r';
}

size_t lookupOperator(const std::u32string& str, size_t pos) {
    switch (str[pos]) {
        case U'+':
            return 1;
        case U'-':
            return 1;
        case U'!':
            if (pos == str.length() - 1) {
                return 1;
            } else if (str[pos + 1] == U'=') {
                return 2;
            }
            return 1;
        case U',':
            return 1;
        case U'%':
            return 1;
        case U'<':
            if (pos == str.length() - 1) {
                return 1;
            } else if (str[pos + 1] == U'=') {
                return 2;
            }
            return 1;
        case U'>':
            if (pos == str.length() - 1) {
                return 1;
            } else if (str[pos + 1] == U'=') {
                return 2;
            }
            return 1;
        case U'=':
            if (pos == str.length() - 1) {
                return 0;
            } else if (str[pos + 1] == U'=') {
                return 1;
            }
            return 0;
        default:
            return 0;
    }
}

LexResult lex(const std::u32string& str) {
    LexResult result;
    result.error = false;
    result.errorPos = 0;
    std::vector<Lexeme>& lexems = result.lexems;
    size_t len = str.length();
    size_t bufStart = 0;
    LexemeType identType;
    LexerState state = LexerState::Undefined;
    for (size_t i = 0; i < len; ++i) {
        char current = str[i];
        switch (state) {
            case LexerState::String:
                if (current == '\"') {
                    Lexeme lexeme;
                    lexeme.type = LexemeType::String;
                    lexeme.val = std::u32string_view(str.data() + bufStart,
                                                     i - bufStart);
                    lexeme.pos = bufStart;
                    lexems.push_back(lexeme);
                    state = LexerState::Undefined;
                } else if (current == '\\') {
                    if (i != len - 1 && str[i + 1] == '\"') {
                        i++;
                    }
                }
                break;
            case LexerState::Identifier:
                if (!isValidIdentChar(current)) {
                    Lexeme lexeme;
                    lexeme.type = identType;
                    lexeme.val = std::u32string_view(str.data() + bufStart,
                                                     i - bufStart);
                    lexeme.pos = bufStart;
                    lexems.push_back(lexeme);
                    state = LexerState::Undefined;
                    i--;
                }
                break;
            case LexerState::Number:
                if (!isNumericChar(current)) {
                    Lexeme lexeme;
                    lexeme.type = LexemeType::Number;
                    lexeme.val = std::u32string_view(str.data() + bufStart,
                                                     i - bufStart);
                    lexeme.pos = bufStart;
                    lexems.push_back(lexeme);
                    state = LexerState::Undefined;
                    i--;
                }
                break;
            case LexerState::Comment:
                if (current == U'\n') {
                    state = LexerState::Undefined;
                }
                break;
            case LexerState::Undefined:
                if (isNumericChar(current)) {
                    state = LexerState::Number;
                    bufStart = i;
                } else if (isValidIdentStart(current)) {
                    state = LexerState::Identifier;
                    identType = LexemeType::Word;
                    bufStart = i;
                } else if (current == U'\"') {
                    if (i == len - 1) {
                        result.errorPos = i;
                        result.error = true;
                        result.errorDescription =
                            U"Missing string literal after \"";
                        result.lexems.clear();
                        return result;
                    }
                    state = LexerState::String;
                    bufStart = i + 1;
                } else if (current == U'[') {
                    Lexeme lexeme;
                    lexeme.pos = i;
                    lexeme.type = LexemeType::OpenSquareBrace;
                    lexems.push_back(lexeme);
                } else if (current == U']') {
                    Lexeme lexeme;
                    lexeme.pos = i;
                    lexeme.type = LexemeType::CloseSquareBrace;
                    lexems.push_back(lexeme);
                } else if (current == U'{') {
                    Lexeme lexeme;
                    lexeme.pos = i;
                    lexeme.type = LexemeType::OpenCurlyBrace;
                    lexems.push_back(lexeme);
                } else if (current == U'}') {
                    Lexeme lexeme;
                    lexeme.pos = i;
                    lexeme.type = LexemeType::CloseCurlyBrace;
                    lexems.push_back(lexeme);
                } else if (lookupOperator(str, i) != 0) {
                    size_t size = lookupOperator(str, i);
                    Lexeme lexeme;
                    lexeme.type = LexemeType::Word;
                    lexeme.pos = i;
                    lexeme.val = std::u32string_view(str.data() + i, size);
                    lexems.push_back(lexeme);
                    i += size - 1;
                } else if (current == U'$') {
                    if (i == len - 1 || !isValidIdentStart(str[i + 1])) {
                        result.errorPos = i;
                        result.error = true;
                        result.errorDescription =
                            U"Missing identifier after $ sign";
                        result.lexems.clear();
                        return result;
                    }
                    identType = LexemeType::WordDeclare;
                    state = LexerState::Identifier;
                    bufStart = ++i;
                } else if (current == U'=') {
                    if (i == len - 1 || !isValidIdentStart(str[i + 1])) {
                        result.errorPos = i;
                        result.error = true;
                        result.errorDescription =
                            U"Missing identifier after = sign";
                        result.lexems.clear();
                        return result;
                    }
                    identType = LexemeType::WordAssignment;
                    state = LexerState::Identifier;
                    bufStart = ++i;
                } else if (current == U'#') {
                    state = LexerState::Comment;
                } else if (!isWhitespace(current)) {
                    result.errorPos = i;
                    result.lexems.clear();
                    return result;
                }
                break;
        }
    }
    switch (state) {
        case LexerState::String:
            result.errorPos = len;
            result.error = true;
            result.errorDescription = U"Not terminated string";
            result.lexems.clear();
            return result;
        case LexerState::Identifier: {
            Lexeme lexeme;
            lexeme.type = LexemeType::Word;
            lexeme.val =
                std::u32string_view(str.data() + bufStart, len - bufStart);
            lexeme.pos = bufStart;
            lexems.push_back(lexeme);
            break;
        }
        case LexerState::Number: {
            Lexeme lexeme;
            lexeme.type = LexemeType::Number;
            lexeme.val =
                std::u32string_view(str.data() + bufStart, len - bufStart);
            lexeme.pos = bufStart;
            lexems.push_back(lexeme);
            break;
        }
        default:
            break;
    }
    return result;
}