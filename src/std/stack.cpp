#include <std/stack.hpp>

ExecutionResult dropOp(Interpreter& interp) {
    if (interp.evalStack.stack.empty()) {
        return ExecutionResult{ExecutionResultType::Error,
                               U"Evaluation stack overflow"};
    }
    // ( a -- )
    interp.evalStack.stack.pop_back();
    return ExecutionResult{ExecutionResultType::Success, U""};
}

ExecutionResult swapOp(Interpreter& interp) {
    if (interp.evalStack.stack.size() < 2) {
        return ExecutionResult{ExecutionResultType::Error,
                               U"Evaluation stack overflow"};
    }
    // ( a b -- b a )
    Value val1 = interp.evalStack.stack.back();  // b
    interp.evalStack.stack.pop_back();
    Value val2 = interp.evalStack.stack.back();  // a
    interp.evalStack.stack.pop_back();
    interp.evalStack.stack.push_back(val1);
    interp.evalStack.stack.push_back(val2);
    return ExecutionResult{ExecutionResultType::Success, U""};
}

ExecutionResult dupOp(Interpreter& interp) {
    if (interp.evalStack.stack.empty()) {
        return ExecutionResult{ExecutionResultType::Error,
                               U"Evaluation stack overflow"};
    }
    // ( a -- a a )
    Value val1 = interp.evalStack.stack.back();  // a
    interp.evalStack.stack.pop_back();
    interp.evalStack.stack.push_back(val1);
    interp.evalStack.stack.push_back(val1);
    return ExecutionResult{ExecutionResultType::Success, U""};
}

ExecutionResult overOp(Interpreter& interp) {
    if (interp.evalStack.stack.size() < 2) {
        return ExecutionResult{ExecutionResultType::Error,
                               U"Evaluation stack overflow"};
    }
    // (a b -- a b a)
    Value val1 = interp.evalStack.stack.back();  // b
    interp.evalStack.stack.pop_back();
    Value val2 = interp.evalStack.stack.back();  // a
    interp.evalStack.stack.pop_back();
    interp.evalStack.stack.push_back(val2);  // a
    interp.evalStack.stack.push_back(val1);  // b
    interp.evalStack.stack.push_back(val2);  // a
    return ExecutionResult{ExecutionResultType::Success, U""};
}

void addStackManipNativeWords(Interpreter& interp) {
    interp.defineNativeWord(U"drop", dropOp);
    interp.defineNativeWord(U"swap", swapOp);
    interp.defineNativeWord(U"over", overOp);
    interp.defineNativeWord(U"dup", dupOp);
}
