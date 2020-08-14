#include <io/lexer.hpp>
#include <io/parser.hpp>
#include <io/strings.hpp>
#include <stack>

ParseResult parse(const std::u32string &str, Heap &h) {
    ParseResult result;
    result.status = ParseResult::Status::Success;

    LexResult lexResult = lex(str);
    if (lexResult.error) {
        result.status = ParseResult::Status::LexerError;
        result.description = std::move(lexResult.errorDescription);
        result.errorPos = lexResult.errorPos;
        result.code = nullptr;
        return result;
    }

    using ParserTask = std::pair<ValueType, Array *>;
    std::stack<ParserTask> tasks;

    result.code = h.makeArrayObject(Value(), 0);
    tasks.push(ParserTask(ValueType::Array, result.code));

    for (size_t i = 0; i < lexResult.lexems.size(); ++i) {
        ParserTask &topTask = tasks.top();
        Lexeme current = lexResult.lexems[i];
        switch (current.type) {
            case LexemeType::OpenSquareBrace:
            case LexemeType::OpenCurlyBrace: {
                Array *newScope = h.makeArrayObject(Value(), 0);
                ValueType correspondingType =
                    (current.type == LexemeType::OpenSquareBrace)
                        ? ValueType::Array
                        : ValueType::Placeholder;
                tasks.push(ParserTask(correspondingType, newScope));
                break;
            }
            case LexemeType::CloseCurlyBrace:
            case LexemeType::CloseSquareBrace: {
                ValueType correspondingType =
                    (current.type == LexemeType::CloseSquareBrace)
                        ? ValueType::Array
                        : ValueType::Placeholder;
                if (topTask.first != correspondingType) {
                    result.status = ParseResult::Status::ParserError;
                    result.description = U"Unmatched array termination";
                    result.errorPos = current.pos;
                    result.code = nullptr;
                    return result;
                }

                Value arrayLiteral;
                arrayLiteral.type = correspondingType;
                arrayLiteral.arr = topTask.second;
                tasks.pop();

                if (tasks.empty()) {
                    result.status = ParseResult::Status::ParserError;
                    result.description = U"Unmatched ] symbol";
                    result.errorPos = current.pos;
                    result.code = nullptr;
                    return result;
                }

                ParserTask &newTopTask = tasks.top();
                newTopTask.second->values.push_back(arrayLiteral);
                break;
            }
            case LexemeType::Number: {
                Value integerLiteral;
                integerLiteral.type = ValueType::Numeric;
                integerLiteral.numericValue = parseIntFrom(current.val);
                topTask.second->values.push_back(integerLiteral);
                break;
            }
            case LexemeType::String: {
                Value stringLiteral;
                stringLiteral.type = ValueType::String;
                String *str = convertFromBackslashed(current.val, h);
                if (str == nullptr) {
                    result.status = ParseResult::Status::ParserError;
                    result.description = U"Invalid string literal";
                    result.errorPos = current.pos;
                    result.code = nullptr;
                    return result;
                }
                stringLiteral.str = str;
                topTask.second->values.push_back(stringLiteral);
                break;
            }
            case LexemeType::Word: {
                Value wordLiteral;
                wordLiteral.type = ValueType::Word;
                wordLiteral.str = h.makeStringObject(current.val);
                topTask.second->values.push_back(wordLiteral);
                break;
            }
            case LexemeType::WordAssignment: {
                Value wordLiteral;
                wordLiteral.type = ValueType::WordAssign;
                wordLiteral.str = h.makeStringObject(current.val);
                topTask.second->values.push_back(wordLiteral);
                break;
            }
            case LexemeType::WordDeclare: {
                Value wordLiteral;
                wordLiteral.type = ValueType::WordDeclare;
                wordLiteral.str = h.makeStringObject(current.val);
                topTask.second->values.push_back(wordLiteral);
                break;
            }
        }
    }

    return result;
}