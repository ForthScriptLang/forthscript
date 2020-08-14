#include <stdint.h>

#include <std/arith.hpp>
#include <std/operators.hpp>

using ArithOperator = int64_t(int64_t, int64_t);

NativeWord makeArithOperator(ArithOperator op) {
    return makeFromBinaryOperator(
        [op](Value val1, Value val2,
             [[maybe_unused]] Interpreter& interp) -> Value {
            if (val1.type != ValueType::Numeric ||
                val2.type != ValueType::Numeric) {
                return Value();
            }
            Value intResult;
            intResult.type = ValueType::Numeric;
            intResult.numericValue = op(val1.numericValue, val2.numericValue);
            return intResult;
        });
}

int64_t subOperator(int64_t a, int64_t b) { return a - b; }
int64_t mulOperator(int64_t a, int64_t b) { return a * b; }
int64_t modOperator(int64_t a, int64_t b) { return a % b; }

Value addOperator(Value val1, Value val2, Interpreter& interp) {
    if (val1.type == ValueType::Numeric && val2.type == ValueType::Numeric) {
        Value intResult;
        intResult.numericValue = val1.numericValue + val2.numericValue;
        return intResult;
    } else if (val1.type == ValueType::String &&
               val2.type == ValueType::String) {
        Value stringResult;
        stringResult.type = ValueType::String;
        std::u32string_view dummy;
        stringResult.str = interp.heap.makeStringObject(dummy);
        stringResult.str->str += val1.str->str;
        stringResult.str->str += val2.str->str;
        return stringResult;
    } else if (val1.type == ValueType::Array && val2.type == ValueType::Array) {
        Value arrayResult;
        arrayResult.type = ValueType::Array;
        arrayResult.arr = interp.heap.makeArrayObject(Value(), 0);
        for (const Value& val : val1.arr->values) {
            arrayResult.arr->values.push_back(val);
        }
        for (const Value& val : val2.arr->values) {
            arrayResult.arr->values.push_back(val);
        }
        return arrayResult;
    }
    return Value();
}

Value divOperator(Value val1, Value val2, [[maybe_unused]] Interpreter&) {
    if (val1.type != ValueType::Numeric || val2.type != ValueType::Numeric) {
        return Value();
    }
    Value intResult;
    intResult.type = ValueType::Numeric;
    if (val2.numericValue == 0) {
        return Value();
    }
    intResult.numericValue = val1.numericValue / val2.numericValue;
    return intResult;
}

void addArithNativeWords(Interpreter& interpreter) {
    interpreter.defineNativeWord(U"+", makeFromBinaryOperator(addOperator));
    interpreter.defineNativeWord(U"-", makeArithOperator(subOperator));
    interpreter.defineNativeWord(U"*", makeArithOperator(mulOperator));
    interpreter.defineNativeWord(U"/", makeFromBinaryOperator(divOperator));
    interpreter.defineNativeWord(U"%", makeArithOperator(modOperator));
}