#include <stdint.h>

#include <io/lexer.hpp>
#include <io/prettyprint.hpp>
#include <io/strings.hpp>
#include <std/operators.hpp>
#include <std/typing.hpp>

Value getTypeOp(Value arg, Interpreter& interp) {
    Value result;
    result.type = ValueType::String;
    switch (arg.type) {
        case ValueType::Array:
            result.str = interp.heap.makeStringObject(U"Array");
            break;
        case ValueType::Boolean:
            result.str = interp.heap.makeStringObject(U"Boolean");
            break;
        case ValueType::NativeWord:
            result.str = interp.heap.makeStringObject(U"NativeWord");
            break;
        case ValueType::Nil:
            result.str = interp.heap.makeStringObject(U"Nil");
            break;
        case ValueType::Numeric:
            result.str = interp.heap.makeStringObject(U"Numeric");
            break;
        case ValueType::Placeholder:
            result.str = interp.heap.makeStringObject(U"Placeholder");
            break;
        case ValueType::String:
            result.str = interp.heap.makeStringObject(U"String");
            break;
        case ValueType::Word:
            result.str = interp.heap.makeStringObject(U"Word");
            break;
        case ValueType::WordAssign:
            result.str = interp.heap.makeStringObject(U"WordAssign");
            break;
        case ValueType::WordDeclare:
            result.str = interp.heap.makeStringObject(U"WordDeclare");
            break;
        case ValueType::NativeHandle:
            result.str = interp.heap.makeStringObject(U"NativeHandle");
            break;
        case ValueType::SplicePlaceholder:
            result.str = interp.heap.makeStringObject(U"SplicePlaceholder");
            break;
    }
    return result;
}

bool validUTF32(char32_t ch);

ExecutionResult intToCharOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(1)) {
        return EvalStackUnderflow();
    }
    Value val = interp.evalStack.popBack().value();
    if (val.type != ValueType::Numeric) {
        return TypeError();
    }
    char32_t arr[2];
    if (val.numericValue < 0 || !validUTF32((char32_t)val.numericValue)) {
        return ExecutionResult{ExecutionResultType::Error,
                               U"Unknown unicode character", Value()};
    }
    arr[0] = (char32_t)(val.numericValue);
    arr[1] = U'\0';

    String* obj = interp.heap.makeStringObject(arr);
    Value result;
    result.type = ValueType::String;
    result.str = obj;
    interp.evalStack.pushBack(result);
    return Success();
}

ExecutionResult charToIntOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(1)) {
        return EvalStackUnderflow();
    }
    Value val = interp.evalStack.popBack().value();
    if (val.type != ValueType::String) {
        return TypeError();
    }
    if (val.str->get().length() != 1) {
        return ExecutionResult{
            ExecutionResultType::Error,
            U"Can't convert not one character string to char", Value()};
    }
    char32_t character = val.str->get()[0];
    Value result;
    result.type = ValueType::Numeric;
    result.numericValue = (int64_t)character;
    interp.evalStack.pushBack(result);
    return Success();
}

ExecutionResult toArrayOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(1)) {
        return EvalStackUnderflow();
    }
    Value val = interp.evalStack.popBack().value();
    if (val.type != ValueType::Placeholder &&
        val.type != ValueType::SplicePlaceholder &&
        val.type != ValueType::Array) {
        return TypeError();
    }
    val.type = ValueType::Array;
    interp.evalStack.pushBack(val);
    return Success();
}

ExecutionResult toPlaceholderOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(1)) {
        return EvalStackUnderflow();
    }
    Value val = interp.evalStack.popBack().value();
    if (val.type != ValueType::Placeholder &&
        val.type != ValueType::SplicePlaceholder &&
        val.type != ValueType::Array) {
        return TypeError();
    }
    val.type = ValueType::Placeholder;
    interp.evalStack.pushBack(val);
    return Success();
}

ExecutionResult toSplicePlaceholderOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(1)) {
        return EvalStackUnderflow();
    }
    Value val = interp.evalStack.popBack().value();
    if (val.type != ValueType::Placeholder &&
        val.type != ValueType::SplicePlaceholder &&
        val.type != ValueType::Array) {
        return TypeError();
    }
    val.type = ValueType::SplicePlaceholder;
    interp.evalStack.pushBack(val);
    return Success();
}

ExecutionResult toIntegerOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(1)) {
        return EvalStackUnderflow();
    }
    Value val = interp.evalStack.popBack().value();
    if (val.type != ValueType::String) {
        return TypeError();
    }
    Value result;
    result.type = ValueType::Numeric;
    size_t i = 0;
    if (val.str->get().empty()) {
        goto failure;
    }
    if (val.str->get() == U"-") {
        goto failure;
    }
    if (val.str->get() == U"0" || val.str->get() == U"-0") {
        result.numericValue = 0;
        goto success;
    }
    if (val.str->get()[0] == U'0' || val.str->get().substr(0, 2) == U"-0") {
        goto success;
    }
    if (val.str->get()[0] == U'-') {
        i = 1;
    }
    for (; i < val.str->get().size(); ++i) {
        if (U'0' > val.str->get()[i] || U'9' < val.str->get()[i]) {
            goto failure;
        }
    }
    result.numericValue = parseIntFrom(val.str->get());
success:
    interp.evalStack.pushBack(result);
    return Success();
failure:
    interp.evalStack.pushBack(Value());
    return Success();
}

ExecutionResult toWordOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(1)) {
        return EvalStackUnderflow();
    }
    Value val = interp.evalStack.popBack().value();
    if (val.type != ValueType::String && val.type != ValueType::Word &&
        val.type != ValueType::WordAssign &&
        val.type != ValueType::WordDeclare) {
        return TypeError();
    }
    if (val.type == ValueType::String) {
        if (val.str->get().empty()) {
            interp.evalStack.pushBack(Value());
            return Success();
        }
        for (size_t i = 0; i < val.str->get().size(); ++i) {
            char32_t c = val.str->get()[i];
            if (i == 0) {
                if (!isValidIdentStart(c)) {
                    interp.evalStack.pushBack(Value());
                    return Success();
                }
            } else if (!isValidIdentChar(c)) {
                interp.evalStack.pushBack(Value());
                return Success();
            }
        }
    }
    val.type = ValueType::Word;
    interp.evalStack.pushBack(val);
    return Success();
}

ExecutionResult toWordAssignOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(1)) {
        return EvalStackUnderflow();
    }
    Value val = interp.evalStack.popBack().value();
    if (val.type != ValueType::String && val.type != ValueType::Word &&
        val.type != ValueType::WordAssign &&
        val.type != ValueType::WordDeclare) {
        return TypeError();
    }
    if (val.type == ValueType::String) {
        if (val.str->get().empty()) {
            interp.evalStack.pushBack(Value());
            return Success();
        }
        for (size_t i = 0; i < val.str->get().size(); ++i) {
            char32_t c = val.str->get()[i];
            if (i == 0) {
                if (!isValidIdentStart(c)) {
                    interp.evalStack.pushBack(Value());
                    return Success();
                }
            } else if (!isValidIdentChar(c)) {
                interp.evalStack.pushBack(Value());
                return Success();
            }
        }
    }
    val.type = ValueType::WordAssign;
    interp.evalStack.pushBack(val);
    return Success();
}

ExecutionResult toWordDeclareOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(1)) {
        return EvalStackUnderflow();
    }
    Value val = interp.evalStack.popBack().value();
    if (val.type != ValueType::String && val.type != ValueType::Word &&
        val.type != ValueType::WordAssign &&
        val.type != ValueType::WordDeclare) {
        return TypeError();
    }
    if (val.type == ValueType::String) {
        if (val.str->get().empty()) {
            interp.evalStack.pushBack(Value());
            return Success();
        }
        for (size_t i = 1; i < val.str->get().size(); ++i) {
            char32_t c = val.str->get()[i];
            if (i == 0) {
                if (!isValidIdentStart(c)) {
                    interp.evalStack.pushBack(Value());
                    return Success();
                }
            } else if (!isValidIdentChar(c)) {
                interp.evalStack.pushBack(Value());
                return Success();
            }
        }
    }
    val.type = ValueType::WordDeclare;
    interp.evalStack.pushBack(val);
    return Success();
}

ExecutionResult toStringOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(1)) {
        return EvalStackUnderflow();
    }
    Value val = interp.evalStack.popBack().value();
    if (val.type == ValueType::String || val.type == ValueType::Word ||
        val.type == ValueType::WordAssign ||
        val.type == ValueType::WordDeclare) {
        val.type = ValueType::String;
    } else {
        String* str = interp.heap.makeStringObject(prettyprint(val, interp));
        val.type = ValueType::String;
        val.str = str;
    }
    interp.evalStack.pushBack(val);
    return Success();
}

ExecutionResult toNativeWordOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(1)) {
        return EvalStackUnderflow();
    }
    Value val = interp.evalStack.popBack().value();
    if (val.type == ValueType::String || val.type == ValueType::Word ||
        val.type == ValueType::WordAssign ||
        val.type == ValueType::WordDeclare) {
        NativeWord word = interp.queryNativeWord(val.str);
        if (word == nullptr) {
            interp.evalStack.pushBack(Value());
            return Success();
        } else {
            val.type = ValueType::NativeWord;
            val.word = word;
            interp.evalStack.pushBack(val);
            return Success();
        }
    }
    return TypeError();
}

MAKE_FROM_UNARY_OPERATOR(typeNativeWord, getTypeOp)

void addTypingNativeWords(Interpreter& interp) {
    interp.defineNativeWord(U"type", typeNativeWord);
    interp.defineNativeWord(U"chr", intToCharOp);
    interp.defineNativeWord(U"ord", charToIntOp);
    interp.defineNativeWord(U"to_numeric", toIntegerOp);
    interp.defineNativeWord(U"to_array", toArrayOp);
    interp.defineNativeWord(U"to_placeholder", toPlaceholderOp);
    interp.defineNativeWord(U"to_slice_placeholder", toSplicePlaceholderOp);
    interp.defineNativeWord(U"to_word", toWordOp);
    interp.defineNativeWord(U"to_word_assign", toWordAssignOp);
    interp.defineNativeWord(U"to_word_declare", toWordDeclareOp);
    interp.defineNativeWord(U"to_string", toStringOp);
    interp.defineNativeWord(U"to_native_word", toNativeWordOp);
}