#include <stdint.h>

#include <expect.hpp>
#include <std/arith.hpp>
#include <std/operators.hpp>

ExecutionResult addOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(2)) {
        return EvalStackUnderflow();
    }
    Value val2 = interp.evalStack.popBack().value();
    Value val1 = interp.evalStack.popBack().value();
    if (val1.type == ValueType::Numeric && val2.type == ValueType::Numeric) {
        Value intResult;
        intResult.type = ValueType::Numeric;
        intResult.numericValue = val1.numericValue + val2.numericValue;
        interp.evalStack.pushBack(intResult);
        return Success();
    } else if (val1.type == ValueType::String &&
               val2.type == ValueType::String) {
        Value stringResult;
        stringResult.type = ValueType::String;
        std::u32string result = val1.str->get() + val2.str->get();
        stringResult.str = interp.heap.makeStringObject(std::move(result));
        interp.evalStack.pushBack(stringResult);
        return Success();
    } else if (val1.type == ValueType::Array && val2.type == ValueType::Array) {
        Value arrayResult;
        arrayResult.type = ValueType::Array;
        arrayResult.arr = interp.heap.makeArrayObject(Value(), 0);
        arrayResult.arr->values.reserve(val1.arr->values.size() +
                                        val2.arr->values.size());
        for (const Value& val : val1.arr->values) {
            arrayResult.arr->values.push_back(val);
        }
        for (const Value& val : val2.arr->values) {
            arrayResult.arr->values.push_back(val);
        }
        interp.evalStack.pushBack(arrayResult);
        return Success();
    }
    return TypeError();
}

ExecutionResult mulOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(2)) {
        return EvalStackUnderflow();
    }
    Value val2 = interp.evalStack.popBack().value();
    Value val1 = interp.evalStack.popBack().value();
    if (val1.type == ValueType::Numeric && val2.type == ValueType::Numeric) {
        Value intResult;
        intResult.type = ValueType::Numeric;
        intResult.numericValue = val1.numericValue * val2.numericValue;
        interp.evalStack.pushBack(intResult);
        return Success();
    } else if (val1.type == ValueType::String &&
               val2.type == ValueType::Numeric) {
        if (val2.numericValue < 0) {
            return ExecutionResult{
                ExecutionResultType::Error,
                U"Multiplication of a string by a negative number", Value()};
        }
        Value stringResult;
        stringResult.type = ValueType::String;
        std::u32string copy;
        copy.reserve(val1.str->get().size() * val2.numericValue);
        for (size_t i = 0; i < (size_t)val2.numericValue; ++i) {
            copy.append(val1.str->get());
        }
        stringResult.str = interp.heap.makeStringObject(std::move(copy));
        interp.evalStack.pushBack(stringResult);
        return Success();
    } else if (val1.type == ValueType::Array &&
               val2.type == ValueType::Numeric) {
        if (val2.numericValue < 0) {
            return ExecutionResult{
                ExecutionResultType::Error,
                U"Multiplication of an array by a negative number", Value()};
        }
        Value arrayResult;
        arrayResult.type = ValueType::Array;
        arrayResult.arr = interp.heap.makeArrayObject(Value(), 0);
        arrayResult.arr->values.reserve(val1.arr->values.size() *
                                        val2.numericValue);
        for (size_t i = 0; i < (size_t)val2.numericValue; ++i) {
            for (const Value& val : val1.arr->values) {
                arrayResult.arr->values.push_back(val);
            }
        }
        interp.evalStack.pushBack(arrayResult);
        return Success();
    }
    return TypeError();
}

ExecutionResult divOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(2)) {
        return EvalStackUnderflow();
    }
    Value val2 = interp.evalStack.popBack().value();
    Value val1 = interp.evalStack.popBack().value();
    if (val1.type != ValueType::Numeric || val2.type != ValueType::Numeric) {
        return TypeError();
    }
    Value result;
    result.type = ValueType::Numeric;
    if (val2.numericValue == 0) {
        return ExecutionResult{ExecutionResultType::Error, U"Division by zero",
                               Value()};
    }
    result.numericValue = val1.numericValue / val2.numericValue;
    interp.evalStack.pushBack(result);
    return Success();
}

ExecutionResult modOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(2)) {
        return EvalStackUnderflow();
    }
    Value val2 = interp.evalStack.popBack().value();
    Value val1 = interp.evalStack.popBack().value();
    if (val1.type != ValueType::Numeric || val2.type != ValueType::Numeric) {
        return TypeError();
    }
    Value result;
    result.type = ValueType::Numeric;
    if (val2.numericValue == 0) {
        return ExecutionResult{ExecutionResultType::Error, U"Division by zero",
                               Value()};
    }
    result.numericValue = val1.numericValue % val2.numericValue;
    interp.evalStack.pushBack(result);
    return Success();
}

ExecutionResult subOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(2)) {
        return EvalStackUnderflow();
    }
    Value val2 = interp.evalStack.popBack().value();
    Value val1 = interp.evalStack.popBack().value();
    if (val1.type != ValueType::Numeric || val2.type != ValueType::Numeric) {
        return TypeError();
    }
    Value result;
    result.type = ValueType::Numeric;
    result.numericValue = val1.numericValue - val2.numericValue;
    interp.evalStack.pushBack(result);
    return Success();
}

void addArithNativeWords(Interpreter& interpreter) {
    interpreter.defineNativeWord(U"+", addOp);
    interpreter.defineNativeWord(U"-", subOp);
    interpreter.defineNativeWord(U"*", mulOp);
    interpreter.defineNativeWord(U"/", divOp);
    interpreter.defineNativeWord(U"%", modOp);
}
