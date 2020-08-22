#include <std/stack.hpp>

ExecutionResult dropOp(Interpreter& interp) {
    if (!interp.evalStack.popBack()) {
        return EvalStackUnderflow();
    }
    return Success();
}

ExecutionResult swapOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(2)) {
        return EvalStackUnderflow();
    }
    Value b = interp.evalStack.popBack().value();
    Value a = interp.evalStack.popBack().value();
    interp.evalStack.pushBack(b);
    interp.evalStack.pushBack(a);
    return Success();
}

ExecutionResult dupOp(Interpreter& interp) {
    std::optional<Value> a = interp.evalStack.popBack();
    if (!a) {
        return EvalStackUnderflow();
    }
    interp.evalStack.pushBack(a.value());
    interp.evalStack.pushBack(a.value());
    return Success();
}

ExecutionResult overOp(Interpreter& interp) {
    std::optional<Value> a, b;
    b = interp.evalStack.popBack();
    a = interp.evalStack.popBack();
    if (!a || !b) {
        return EvalStackUnderflow();
    }
    interp.evalStack.pushBack(a.value());
    interp.evalStack.pushBack(b.value());
    interp.evalStack.pushBack(a.value());
    return Success();
}

ExecutionResult clearOp(Interpreter& interp) {
    interp.evalStack.clear();
    return Success();
}

void addStackManipNativeWords(Interpreter& interp) {
    interp.defineNativeWord(U"drop", dropOp);
    interp.defineNativeWord(U"swap", swapOp);
    interp.defineNativeWord(U"over", overOp);
    interp.defineNativeWord(U"dup", dupOp);
    interp.defineNativeWord(U"clear", clearOp);
}
