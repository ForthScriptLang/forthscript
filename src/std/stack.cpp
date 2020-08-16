#include <std/stack.hpp>

ExecutionResult dropOp(Interpreter& interp) {
    if (!interp.evalStack.popBack()) {
        return ExecutionResult{ExecutionResultType::Error,
                               U"Evaluation stack overflow"};
    }
    return ExecutionResult{ExecutionResultType::Success, U""};
}

ExecutionResult swapOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(2)) {
        return ExecutionResult{ExecutionResultType::Error,
                               U"Evaluation stack overflow"};
    }
    Value b = interp.evalStack.popBack().value();
    Value a = interp.evalStack.popBack().value();
    interp.evalStack.pushBack(b);
    interp.evalStack.pushBack(a);
    return ExecutionResult{ExecutionResultType::Success, U""};
}

ExecutionResult dupOp(Interpreter& interp) {
    std::optional<Value> a = interp.evalStack.popBack();
    if (!a) {
        return ExecutionResult{ExecutionResultType::Error,
                               U"Evaluation stack overflow"};
    }
    interp.evalStack.pushBack(a.value());
    interp.evalStack.pushBack(a.value());
    return ExecutionResult{ExecutionResultType::Success, U""};
}

ExecutionResult overOp(Interpreter& interp) {
    std::optional<Value> a, b;
    b = interp.evalStack.popBack();
    a = interp.evalStack.popBack();
    if (!a || !b) {
        return ExecutionResult{ExecutionResultType::Error,
                               U"Evaluation stack overflow"};
    }
    interp.evalStack.pushBack(a.value());
    interp.evalStack.pushBack(b.value());
    interp.evalStack.pushBack(a.value());
    return ExecutionResult{ExecutionResultType::Success, U""};
}

void addStackManipNativeWords(Interpreter& interp) {
    interp.defineNativeWord(U"drop", dropOp);
    interp.defineNativeWord(U"swap", swapOp);
    interp.defineNativeWord(U"over", overOp);
    interp.defineNativeWord(U"dup", dupOp);
}
