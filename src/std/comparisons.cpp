#include <expect.hpp>
#include <std/comparisons.hpp>
#include <std/operators.hpp>

#define MAKE_COMP_OPERATOR(name, op)                                    \
    ExecutionResult name(Interpreter& interp) {                         \
        if_unlikely(!interp.evalStack.assertDepth(2)) {                 \
            return EvalStackUnderflow();                                \
        }                                                               \
        Value val2 = interp.evalStack.popBack().value();                \
        Value val1 = interp.evalStack.popBack().value();                \
        if (val1.type != ValueType::Numeric ||                          \
            val2.type != ValueType::Numeric) {                          \
            return TypeError();                                         \
        }                                                               \
        Value result;                                                   \
        result.type = ValueType::Boolean;                               \
        result.booleanValue = op(val1.numericValue, val2.numericValue); \
        interp.evalStack.pushBack(result);                              \
        return Success();                                               \
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
        case ValueType::SplicePlaceholder:
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
            result.booleanValue = val1.str == val2.str;
            break;
        case ValueType::NativeWord:
            result.booleanValue = val1.word == val2.word;
            break;
        case ValueType::Nil:
            result.booleanValue = true;
            break;
        case ValueType::NativeHandle:
            result.booleanValue = val1.handle == val2.handle;
            break;
    }
    return result;
}

Value neOperator(Value val1, Value val2, Interpreter& interp) {
    Value result = eqOperator(val1, val2, interp);
    result.booleanValue = !result.booleanValue;
    return result;
}

MAKE_FROM_BINARY_OPERATOR(eqNativeWord, eqOperator)
MAKE_FROM_BINARY_OPERATOR(neNativeWord, neOperator)

bool ltOperator(int64_t num1, int64_t num2) { return num1 < num2; }
bool leOperator(int64_t num1, int64_t num2) { return num1 <= num2; }
bool gtOperator(int64_t num1, int64_t num2) { return num1 > num2; }
bool geOperator(int64_t num1, int64_t num2) { return num1 >= num2; }

MAKE_COMP_OPERATOR(ltNativeWord, ltOperator)
MAKE_COMP_OPERATOR(leNativeWord, leOperator)
MAKE_COMP_OPERATOR(gtNativeWord, gtOperator)
MAKE_COMP_OPERATOR(geNativeWord, geOperator)

void addComparisonsNativeWords(Interpreter& interpreter) {
    interpreter.defineNativeWord(U"<", ltNativeWord);
    interpreter.defineNativeWord(U"<=", leNativeWord);
    interpreter.defineNativeWord(U">", gtNativeWord);
    interpreter.defineNativeWord(U">=", geNativeWord);
    interpreter.defineNativeWord(U"==", eqNativeWord);
    interpreter.defineNativeWord(U"!=", neNativeWord);
}
