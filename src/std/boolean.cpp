#include <std/boolean.hpp>

ExecutionResult andOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(2)) {
        return EvalStackUnderflow();
    }
    Value val1 = interp.evalStack.popBack().value();
    Value val2 = interp.evalStack.popBack().value();
    if (val1.type != ValueType::Boolean || val2.type != ValueType::Boolean) {
        return TypeError();
    }
    Value result;
    result.type = ValueType::Boolean;
    result.booleanValue = val1.booleanValue && val2.booleanValue;
    interp.evalStack.pushBack(result);
    return Success();
}

ExecutionResult orOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(2)) {
        return EvalStackUnderflow();
    }
    Value val1 = interp.evalStack.popBack().value();
    Value val2 = interp.evalStack.popBack().value();
    if (val1.type != ValueType::Boolean || val2.type != ValueType::Boolean) {
        return TypeError();
    }
    Value result;
    result.type = ValueType::Boolean;
    result.booleanValue = val1.booleanValue || val2.booleanValue;
    interp.evalStack.pushBack(result);
    return Success();
}

ExecutionResult notOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(2)) {
        return EvalStackUnderflow();
    }
    Value val = interp.evalStack.popBack().value();
    if (val.type != ValueType::Boolean) {
        return TypeError();
    }
    val.booleanValue = !val.booleanValue;
    interp.evalStack.pushBack(val);
    return Success();
}

void addBooleanNativeWords(Interpreter& interp) {
    interp.defineNativeWord(U"and", andOp);
    interp.defineNativeWord(U"or", orOp);
    interp.defineNativeWord(U"not", notOp);
}