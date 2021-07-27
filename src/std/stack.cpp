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

ExecutionResult getStackCopyOp(Interpreter& interp) {
    const std::vector<Value>& stack = interp.evalStack.getStack();
    Array* result = interp.heap.makeArrayObject(Value{}, stack.size());
    for (size_t i = 0; i < stack.size(); ++i) {
        result->values[i] = stack[i];
    }
    Value stackCopy;
    stackCopy.type = ValueType::Array;
    stackCopy.arr = result;
    interp.evalStack.pushBack(stackCopy);
    return Success();
}

ExecutionResult getStackSizeOp(Interpreter& interp) {
    Value result;
    result.type = ValueType::Numeric;
    result.numericValue = interp.evalStack.getStackSize();
    interp.evalStack.pushBack(result);
    return Success();
}

void addStackManipNativeWords(Interpreter& interp) {
    interp.defineNativeWord(U"get_stack_size", getStackSizeOp);
    interp.defineNativeWord(U"get_stack_copy", getStackCopyOp);
    interp.defineNativeWord(U"drop", dropOp);
    interp.defineNativeWord(U"swap", swapOp);
    interp.defineNativeWord(U"over", overOp);
    interp.defineNativeWord(U"dup", dupOp);
    interp.defineNativeWord(U"clear", clearOp);
}
