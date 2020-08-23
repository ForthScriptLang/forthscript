#include <stdint.h>

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
    }
    return result;
}

ExecutionResult intToCharOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(1)) {
        return EvalStackUnderflow();
    }
    Value val = interp.evalStack.popBack().value();
    if (val.type != ValueType::Numeric) {
        return TypeError();
    }
    char32_t arr[2];
    if (val.numericValue > UINT32_MAX || val.numericValue < 0) {
        return ExecutionResult{ExecutionResultType::Error,
                               U"Unable to cast to char", Value()};
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

MAKE_FROM_UNARY_OPERATOR(typeNativeWord, getTypeOp)

void addTypingNativeWords(Interpreter& interp) {
    interp.defineNativeWord(U"type", typeNativeWord);
    interp.defineNativeWord(U"chr", intToCharOp);
    interp.defineNativeWord(U"ord", charToIntOp);
}