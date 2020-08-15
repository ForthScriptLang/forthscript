#include <std/operators.hpp>

NativeWord makeFromUnaryOperator(UnaryOperator op) {
    return [op](struct Interpreter& interp) {
        if (interp.evalStack.stack.size() < 1) {
            return ExecutionResult{ExecutionResultType::Error,
                                   U"Evaluation stack underflow"};
        }
        Value& first =
            interp.evalStack.stack[interp.evalStack.stack.size() - 1];
        Value result = op(first, interp);
        interp.evalStack.stack.pop_back();
        interp.evalStack.stack.push_back(result);
        return ExecutionResult{ExecutionResultType::Success,
                               U"Evaluation stack underflow"};
    };
}

NativeWord makeFromBinaryOperator(BinaryOperator op) {
    return [op](struct Interpreter& interp) {
        if (interp.evalStack.stack.size() < 2) {
            return ExecutionResult{ExecutionResultType::Error,
                                   U"Evaluation stack underflow"};
        }
        Value& first =
            interp.evalStack.stack[interp.evalStack.stack.size() - 2];
        Value& second =
            interp.evalStack.stack[interp.evalStack.stack.size() - 1];
        Value result = op(first, second, interp);
        interp.evalStack.stack.pop_back();
        interp.evalStack.stack.pop_back();
        interp.evalStack.stack.push_back(result);
        return ExecutionResult{ExecutionResultType::Success, U""};
    };
}

NativeWord makeFromTernaryOperator(TernaryOperator op) {
    return [op](struct Interpreter& interp) {
        if (interp.evalStack.stack.size() < 2) {
            return ExecutionResult{ExecutionResultType::Error,
                                   U"Evaluation stack underflow"};
        }
        Value& first =
            interp.evalStack.stack[interp.evalStack.stack.size() - 3];
        Value& second =
            interp.evalStack.stack[interp.evalStack.stack.size() - 2];
        Value& third =
            interp.evalStack.stack[interp.evalStack.stack.size() - 1];
        Value result = op(first, second, third, interp);
        interp.evalStack.stack.pop_back();
        interp.evalStack.stack.pop_back();
        interp.evalStack.stack.pop_back();
        interp.evalStack.stack.push_back(result);
        return ExecutionResult{ExecutionResultType::Success, U""};
    };
}