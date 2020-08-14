#include <std/stack.hpp>

ExecutionResult dropOp(Interpreter& interp) {
    if (interp.evalStack.stack.empty()) {
        return ExecutionResult{ExecutionResultType::Error,
                               ExecutionErrorType::EvalStackUnderflow};
    }
    // ( a -- )
    interp.evalStack.stack.pop_back();
    return ExecutionResult{ExecutionResultType::Success,
                           ExecutionErrorType::Success};
}

ExecutionResult swapOp(Interpreter& interp) {
    if (interp.evalStack.stack.size() < 2) {
        return ExecutionResult{ExecutionResultType::Error,
                               ExecutionErrorType::EvalStackUnderflow};
    }
    // ( a b -- b a )
    Value val1 = interp.evalStack.stack.back();  // b
    interp.evalStack.stack.pop_back();
    Value val2 = interp.evalStack.stack.back();  // a
    interp.evalStack.stack.pop_back();
    interp.evalStack.stack.push_back(val1);
    interp.evalStack.stack.push_back(val2);
    return ExecutionResult{ExecutionResultType::Success,
                           ExecutionErrorType::Success};
}

ExecutionResult dupOp(Interpreter& interp) {
    if (interp.evalStack.stack.empty()) {
        return ExecutionResult{ExecutionResultType::Error,
                               ExecutionErrorType::EvalStackUnderflow};
    }
    // ( a -- a a )
    Value val1 = interp.evalStack.stack.back();  // a
    interp.evalStack.stack.pop_back();
    interp.evalStack.stack.push_back(val1);
    interp.evalStack.stack.push_back(val1);
    return ExecutionResult{ExecutionResultType::Success,
                           ExecutionErrorType::Success};
}

ExecutionResult overOp(Interpreter& interp) {
    if (interp.evalStack.stack.size() < 2) {
        return ExecutionResult{ExecutionResultType::Error,
                               ExecutionErrorType::EvalStackUnderflow};
    }
    // (a b -- a b a)
    Value val1 = interp.evalStack.stack.back();  // b
    interp.evalStack.stack.pop_back();
    Value val2 = interp.evalStack.stack.back();  // a
    interp.evalStack.stack.pop_back();
    interp.evalStack.stack.push_back(val2);  // a
    interp.evalStack.stack.push_back(val1);  // b
    interp.evalStack.stack.push_back(val2);  // a
    return ExecutionResult{ExecutionResultType::Success,
                           ExecutionErrorType::Success};
}

void addStackManipNativeWords(Interpreter& interp) {
    interp.defineNativeWord(U"drop", dropOp);
    interp.defineNativeWord(U"swap", swapOp);
    interp.defineNativeWord(U"over", overOp);
    interp.defineNativeWord(U"dup", dupOp);
}
