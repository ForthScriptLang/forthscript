#include <io/lexer.hpp>
#include <io/parser.hpp>
#include <io/strings.hpp>
#include <stack>

ValueType getBraceType(LexemeType type) {
    if (type == LexemeType::OpenCircleBrace ||
        type == LexemeType::CloseCircleBrace) {
        return ValueType::SplicePlaceholder;
    } else if (type == LexemeType::OpenCurlyBrace ||
               type == LexemeType::CloseCurlyBrace) {
        return ValueType::Placeholder;
    }
    return ValueType::Array;
}

ParseResult parse(const std::u32string &str, Interpreter &interp) {
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

    result.code = interp.heap.makeArrayObject(Value(), 0);
    tasks.push(ParserTask(ValueType::Array, result.code));

    for (size_t i = 0; i < lexResult.lexems.size(); ++i) {
        ParserTask &topTask = tasks.top();
        Lexeme current = lexResult.lexems[i];
        switch (current.type) {
            case LexemeType::OpenSquareBrace:
            case LexemeType::OpenCurlyBrace:
            case LexemeType::OpenCircleBrace: {
                Array *newScope = interp.heap.makeArrayObject(Value(), 0);
                ValueType correspondingType = getBraceType(current.type);
                tasks.push(ParserTask(correspondingType, newScope));
                break;
            }
            case LexemeType::CloseCircleBrace:
            case LexemeType::CloseCurlyBrace:
            case LexemeType::CloseSquareBrace: {
                ValueType correspondingType = getBraceType(current.type);
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
                if (current.val.size() >= 2 && current.val != U"-0" &&
                    current.val.substr(0, 2) == U"-0") {
                    result.status = ParseResult::Status::ParserError;
                    result.description = U"Leading zeros are not permitted";
                    result.errorPos = current.pos;
                    result.code = nullptr;
                    return result;
                } else if (current.val.size() >= 1 && current.val != U"0" &&
                           current.val[0] == U'0') {
                    result.status = ParseResult::Status::ParserError;
                    result.description = U"Leading zeros are not permitted";
                    result.errorPos = current.pos;
                    result.code = nullptr;
                    return result;
                }
                integerLiteral.numericValue = parseIntFrom(current.val);
                topTask.second->values.push_back(integerLiteral);
                break;
            }
            case LexemeType::String: {
                Value stringLiteral;
                stringLiteral.type = ValueType::String;
                String *strObject =
                    convertFromBackslashed(current.val, interp.heap);
                if (strObject == nullptr) {
                    result.status = ParseResult::Status::ParserError;
                    result.description = U"Invalid string literal";
                    result.errorPos = current.pos;
                    result.code = nullptr;
                    return result;
                }
                stringLiteral.str = strObject;
                topTask.second->values.push_back(stringLiteral);
                break;
            }
            case LexemeType::Word: {
                if (current.val == U"True" || current.val == U"False") {
                    Value boolLiteral;
                    boolLiteral.type = ValueType::Boolean;
                    boolLiteral.booleanValue = current.val == U"True";
                    topTask.second->values.push_back(boolLiteral);
                } else {
                    Value wordLiteral;
                    String *name = interp.heap.makeStringObject(current.val);
                    NativeWord nativeWord = interp.queryNativeWord(name);
                    if (nativeWord != nullptr) {
                        wordLiteral.type = ValueType::NativeWord;
                        wordLiteral.word = nativeWord;
                    } else {
                        wordLiteral.type = ValueType::Word;
                        wordLiteral.str = name;
                    }
                    topTask.second->values.push_back(wordLiteral);
                }
                break;
            }
            case LexemeType::WordAssignment: {
                Value wordLiteral;
                wordLiteral.type = ValueType::WordAssign;
                wordLiteral.str = interp.heap.makeStringObject(current.val);
                topTask.second->values.push_back(wordLiteral);
                break;
            }
            case LexemeType::WordDeclare: {
                Value wordLiteral;
                wordLiteral.type = ValueType::WordDeclare;
                wordLiteral.str = interp.heap.makeStringObject(current.val);
                topTask.second->values.push_back(wordLiteral);
                break;
            }
        }
    }
    if (tasks.size() > 1) {
        result.status = ParseResult::Status::ParserError;
        result.description = U"Close brackets missing";
        result.errorPos = str.size();
        result.code = nullptr;
        return result;
    }
    return result;
}