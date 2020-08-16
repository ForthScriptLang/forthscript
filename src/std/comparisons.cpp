#include <std/comparisons.hpp>
#include <std/operators.hpp>

using CompOperator = bool(int64_t, int64_t);

NativeWord makeCompOperator(CompOperator op) {
    return makeFromBinaryOperator(
        [op](Value val1, Value val2,
             [[maybe_unused]] Interpreter& interp) -> Value {
            if (val1.type != ValueType::Numeric ||
                val2.type != ValueType::Numeric) {
                return Value();
            }
            Value intResult;
            intResult.type = ValueType::Boolean;
            intResult.numericValue = op(val1.numericValue, val2.numericValue);
            return intResult;
        });
}

Value eqOperator(Value val1, Value val2, [[maybe_unused]] Interpreter& interp) {
    Value result;
    result.type = ValueType::Boolean;
    if (val1.type != val2.type) {
        result.booleanValue = false;
        return result;
    }
    switch (val1.type) {
        case ValueType::Array:
        case ValueType::Placeholder:
            result.booleanValue = val1.arr == val2.arr;
            break;
        case ValueType::Numeric:
            result.booleanValue = val1.numericValue == val2.numericValue;
            break;
        case ValueType::Boolean:
            result.booleanValue = val1.booleanValue == val2.booleanValue;
            break;
        case ValueType::String:
        case ValueType::Word:
        case ValueType::WordAssign:
        case ValueType::WordDeclare:
            result.booleanValue = val1.str->get() == val2.str->get();
            break;
        case ValueType::Nil:
            result.booleanValue = true;
            break;
    }
    return result;
}

Value neOperator(Value val1, Value val2, Interpreter& interp) {
    Value result = eqOperator(val1, val2, interp);
    result.booleanValue = !result.booleanValue;
    return result;
}

bool ltOperator(int64_t num1, int64_t num2) { return num1 < num2; }
bool leOperator(int64_t num1, int64_t num2) { return num1 <= num2; }
bool gtOperator(int64_t num1, int64_t num2) { return num1 > num2; }
bool geOperator(int64_t num1, int64_t num2) { return num1 >= num2; }

void addComparisonsNativeWords(Interpreter& interpreter) {
    interpreter.defineNativeWord(U"<", makeCompOperator(ltOperator));
    interpreter.defineNativeWord(U"<=", makeCompOperator(leOperator));
    interpreter.defineNativeWord(U">", makeCompOperator(gtOperator));
    interpreter.defineNativeWord(U">=", makeCompOperator(geOperator));
    interpreter.defineNativeWord(U"==", makeFromBinaryOperator(eqOperator));
    interpreter.defineNativeWord(U"!=", makeFromBinaryOperator(neOperator));
}
