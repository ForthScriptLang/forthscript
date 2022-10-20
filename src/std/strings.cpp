#include <io/parser.hpp>
#include <io/prettyprint.hpp>
#include <sstream>
#include <std/operators.hpp>
#include <std/strings.hpp>
#include <vector>

Value toStringOp(Value val, Interpreter& interp) {
    std::u32string resultCppStr = prettyprint(val, interp);
    Value result;
    result.type = ValueType::String;
    result.str = interp.heap.makeStringObject(std::move(resultCppStr));
    return result;
}

ExecutionResult fromStringOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(1)) {
        return EvalStackUnderflow();
    }
    Value val = interp.evalStack.popBack().value();
    if (val.type != ValueType::String) {
        return TypeError();
    }
    ParseResult result = parse(val.str->get(), interp);
    if (result.isError()) {
        interp.heap.collectGarbage();
        return ExecutionResult{ExecutionResultType::Error,
                               U"Failed to parse input", Value()};
    }
    Value valueResult;
    valueResult.arr = result.code;
    valueResult.type = ValueType::Array;
    interp.evalStack.pushBack(valueResult);
    return Success();
}

ExecutionResult splitOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(2)) {
        return EvalStackUnderflow();
    }
    Value delString = interp.evalStack.popBack().value();
    Value splitString = interp.evalStack.popBack().value();
    if (delString.type != ValueType::String ||
        splitString.type != ValueType::String) {
        return TypeError();
    }
    Value result;
    result.type = ValueType::Array;
    result.arr = interp.heap.makeArrayObject(Value(), 0);
    size_t start = 0;
    for (size_t i = 0; i < splitString.str->get().size(); ++i) {
        char32_t ch = splitString.str->get()[i];
        for (size_t j = 0; j < delString.str->get().size(); ++j) {
            if (delString.str->get()[j] == ch) {
                std::u32string substr =
                    splitString.str->get().substr(start, i - start);
                start = i + 1;
                String* str = interp.heap.makeStringObject(substr);
                Value val;
                val.str = str;
                val.type = ValueType::String;
                result.arr->values.push_back(val);
            }
        }
    }
    if (start < splitString.str->get().size()) {
        std::u32string substr = splitString.str->get().substr(
            start, splitString.str->get().size() - start);
        String* str = interp.heap.makeStringObject(substr);
        Value val;
        val.str = str;
        val.type = ValueType::String;
        result.arr->values.push_back(val);
    }
    interp.evalStack.pushBack(result);
    return Success();
}

ExecutionResult joinOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(2)) {
        return EvalStackUnderflow();
    }
    Value sepString = interp.evalStack.popBack().value();
    Value strings = interp.evalStack.popBack().value();
    if (sepString.type != ValueType::String ||
        strings.type != ValueType::Array) {
        return TypeError();
    }
    for (size_t i = 0; i < strings.arr->values.size(); ++i) {
        if (strings.arr->values[i].type != ValueType::String) {
            return TypeError();
        }
    }
    Value result;
    result.type = ValueType::String;
    if (strings.arr->values.size() == 0) {
        result.str = interp.heap.makeStringObject(U"");
        interp.evalStack.pushBack(result);
        return Success();
    }
    std::u32string str = strings.arr->values[0].str->get();
    for (size_t i = 1; i < strings.arr->values.size(); ++i) {
        str.append(sepString.str->get());
        str.append(strings.arr->values[i].str->get());
    }
    result.str = interp.heap.makeStringObject(str);
    interp.evalStack.pushBack(result);
    return Success();
}

MAKE_FROM_UNARY_OPERATOR(toStringNativeWord, toStringOp)

void addStringManipulationNativeWords(Interpreter& interp) {
    interp.defineNativeWord(U"serialize", toStringNativeWord);
    interp.defineNativeWord(U"parse", fromStringOp);
    interp.defineNativeWord(U"join", joinOp);
    interp.defineNativeWord(U"split", splitOp);
}
