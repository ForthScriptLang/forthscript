#include <io/parser.hpp>
#include <io/prettyprint.hpp>
#include <std/operators.hpp>
#include <std/strings.hpp>

Value toStringOp(Value val, Interpreter& interp) {
    std::u32string resultCppStr = prettyprint(val, interp);
    Value result;
    result.type = ValueType::String;
    result.str = interp.heap.makeStringObject(std::move(resultCppStr));
    return result;
}

Value fromStringOp(Value val, Interpreter& interp) {
    if (val.type != ValueType::String) {
        return Value();
    }
    ParseResult result = parse(val.str->get(), interp);
    if (result.isError()) {
        interp.heap.collectGarbage();
        return Value();
    }
    Value valueResult;
    valueResult.arr = result.code;
    valueResult.type = ValueType::Array;
    return valueResult;
}

MAKE_FROM_UNARY_OPERATOR(toStringNativeWord, toStringOp)
MAKE_FROM_UNARY_OPERATOR(fromStringNativeWord, fromStringOp)

void addStringManipulationNativeWords(Interpreter& interp) {
    interp.defineNativeWord(U"to_string", toStringNativeWord);
    interp.defineNativeWord(U"from_string", fromStringNativeWord);
}