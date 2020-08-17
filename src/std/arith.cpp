#include <stdint.h>

#include <expect.hpp>
#include <std/arith.hpp>
#include <std/operators.hpp>

Value addOperator(Value val1, Value val2, Interpreter& interp) {
    if (val1.type == ValueType::Numeric && val2.type == ValueType::Numeric) {
        Value intResult;
        intResult.type = ValueType::Numeric;
        intResult.numericValue = val1.numericValue + val2.numericValue;
        return intResult;
    } else if (val1.type == ValueType::String &&
               val2.type == ValueType::String) {
        Value stringResult;
        stringResult.type = ValueType::String;
        std::u32string result = val1.str->get() + val2.str->get();
        stringResult.str = interp.heap.makeStringObject(result);
        return stringResult;
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
        return arrayResult;
    }
    return Value();
}

Value mulOperator(Value val1, Value val2, Interpreter& interp) {
    if (val1.type == ValueType::Numeric && val2.type == ValueType::Numeric) {
        Value intResult;
        intResult.type = ValueType::Numeric;
        intResult.numericValue = val1.numericValue * val2.numericValue;
        return intResult;
    } else if (val1.type == ValueType::String &&
               val2.type == ValueType::Numeric) {
        if (val2.numericValue < 0) {
            return Value();
        }
        Value stringResult;
        stringResult.type = ValueType::String;
        std::u32string copy;
        copy.reserve(val1.str->get().size() * val2.numericValue);
        for (size_t i = 0; i < (size_t)val2.numericValue; ++i) {
            copy.append(val1.str->get());
        }
        stringResult.str = interp.heap.makeStringObject(copy);
        return stringResult;
    } else if (val1.type == ValueType::Array &&
               val2.type == ValueType::Numeric) {
        if (val2.numericValue < 0) {
            return Value();
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
        return arrayResult;
    }
    return Value();
}

Value divOperator(Value val1, Value val2, [[maybe_unused]] Interpreter&) {
    if_unlikely(val1.type != ValueType::Numeric ||
                val2.type != ValueType::Numeric) {
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

Value modOperator(Value val1, Value val2, [[maybe_unused]] Interpreter&) {
    if_unlikely(val1.type != ValueType::Numeric ||
                val2.type != ValueType::Numeric) {
        return Value();
    }
    Value intResult;
    intResult.type = ValueType::Numeric;
    if (val2.numericValue == 0) {
        return Value();
    }
    intResult.numericValue = val1.numericValue % val2.numericValue;
    return intResult;
}

Value subOperator(Value val1, Value val2, [[maybe_unused]] Interpreter&) {
    if_unlikely(val1.type != ValueType::Numeric ||
                val2.type != ValueType::Numeric) {
        return Value();
    }
    Value intResult;
    intResult.type = ValueType::Numeric;
    intResult.numericValue = val1.numericValue - val2.numericValue;
    return intResult;
}

MAKE_FROM_BINARY_OPERATOR(addNativeWord, addOperator)
MAKE_FROM_BINARY_OPERATOR(mulNativeWord, mulOperator)
MAKE_FROM_BINARY_OPERATOR(divNativeWord, divOperator)
MAKE_FROM_BINARY_OPERATOR(modNativeWord, modOperator)
MAKE_FROM_BINARY_OPERATOR(subNativeWord, subOperator)

void addArithNativeWords(Interpreter& interpreter) {
    interpreter.defineNativeWord(U"+", addNativeWord);
    interpreter.defineNativeWord(U"-", subNativeWord);
    interpreter.defineNativeWord(U"*", mulNativeWord);
    interpreter.defineNativeWord(U"/", divNativeWord);
    interpreter.defineNativeWord(U"%", modNativeWord);
}