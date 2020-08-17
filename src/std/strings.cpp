#include <io/parser.hpp>
#include <io/prettyprint.hpp>
#include <std/operators.hpp>
#include <std/strings.hpp>

Value toStringOp(Value val, Interpreter& interp) {
    std::u32string resultCppStr = prettyprint(val);
    Value result;
    result.type = ValueType::String;
    result.str = interp.heap.makeStringObject(std::move(resultCppStr));
    return result;
}

MAKE_FROM_UNARY_OPERATOR(toStringNativeWord, toStringOp)

void addStringManipulationNativeWords(Interpreter& interp) {
    interp.defineNativeWord(U"to_string", toStringNativeWord);
}