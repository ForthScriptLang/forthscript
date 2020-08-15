#include <io/parser.hpp>
#include <io/prettyprint.hpp>
#include <std/operators.hpp>
#include <std/strings.hpp>

Value toStringOp(Value val, Interpreter& interp) {
    std::u32string resultCppStr = prettyprint(val);
    std::u32string_view dummy;
    String* resultStr = interp.heap.makeStringObject(dummy);
    resultStr->str = std::move(resultCppStr);
    Value result;
    result.type = ValueType::String;
    result.str = resultStr;
    return result;
}

void addStringManipulationNativeWords(Interpreter& interp) {
    interp.defineNativeWord(U"to_string", makeFromUnaryOperator(toStringOp));
}