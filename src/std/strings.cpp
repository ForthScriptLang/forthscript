#include <io/parser.hpp>
#include <io/prettyprint.hpp>
#include <std/operators.hpp>
#include <std/strings.hpp>

Value toStringOp(Value val, Interpreter& interp) {
    Value result;
    result.type = ValueType::String;
    result.str = interp.heap.makeStringObject(prettyprint(val));
    return result;
}

void addStringManipulationNativeWords(Interpreter& interp) {
    interp.defineNativeWord(U"to_string", makeFromUnaryOperator(toStringOp));
}