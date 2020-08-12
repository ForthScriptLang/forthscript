#include <core/callstack.hpp>
#include <core/interpreter.hpp>
#include <core/symtable.hpp>
#include <core/values.hpp>
#include <dbg.hpp>
#include <heap.hpp>
#include <iostream>

int main() {
    Interpreter interp(1024);
    Array* code = interp.heap.makeArrayObject(Value(), 3);
    code->values[0].type = code->values[1].type = ValueType::Numeric;
    code->values[0].numericValue = code->values[1].numericValue = 2;
    code->values[2].type = ValueType::Word;
    code->values[2].str = interp.heap.makeStringObject(U"+");
    interp.defineNativeWord(U"+", [](Interpreter& interp) {
        if (interp.evalStack.stack.size() < 2) {
            return ExecutionResult{ExecutionResultType::Error,
                                   ExecutionErrorType::EvalStackUnderflow};
        }
        Value& first =
            interp.evalStack.stack[interp.evalStack.stack.size() - 2];
        Value& second =
            interp.evalStack.stack[interp.evalStack.stack.size() - 1];
        if (first.type != ValueType::Numeric &&
            second.type != ValueType::Numeric) {
            return ExecutionResult{ExecutionResultType::Error,
                                   ExecutionErrorType::EvalStackUnderflow};
        }
        Value result;
        result.type = ValueType::Numeric;
        result.numericValue = first.numericValue + second.numericValue;
        interp.evalStack.stack.pop_back();
        interp.evalStack.stack.pop_back();
        interp.evalStack.stack.push_back(result);
        return ExecutionResult{ExecutionResultType::Error,
                               ExecutionErrorType::EvalStackUnderflow};
    });
    interp.callInterpreter(code);
    std::cout << interp.evalStack.stack[0].numericValue << std::endl;
    return 0;
}