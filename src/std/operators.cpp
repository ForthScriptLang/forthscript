#include <std/operators.hpp>

NativeWord makeFromUnaryOperator(UnaryOperator op) {
    return [op](struct Interpreter& interp) {
        if (!interp.evalStack.assertDepth(1)) {
            return ExecutionResult{ExecutionResultType::Error,
                                   U"Evaluation stack underflow"};
        }
        Value first = interp.evalStack.popBack().value();
        Value result = op(first, interp);
        interp.evalStack.pushBack(result);
        return ExecutionResult{ExecutionResultType::Success, U""};
    };
}

NativeWord makeFromBinaryOperator(BinaryOperator op) {
    return [op](struct Interpreter& interp) {
        if (!interp.evalStack.assertDepth(2)) {
            return ExecutionResult{ExecutionResultType::Error,
                                   U"Evaluation stack underflow"};
        }
        Value second = interp.evalStack.popBack().value(),
              first = interp.evalStack.popBack().value();
        Value result = op(first, second, interp);
        interp.evalStack.pushBack(result);
        return ExecutionResult{ExecutionResultType::Success, U""};
    };
}

NativeWord makeFromTernaryOperator(TernaryOperator op) {
    return [op](struct Interpreter& interp) {
        if (!interp.evalStack.assertDepth(3)) {
            return ExecutionResult{ExecutionResultType::Error,
                                   U"Evaluation stack underflow"};
        }
        Value third = interp.evalStack.popBack().value(),
              second = interp.evalStack.popBack().value(),
              first = interp.evalStack.popBack().value();
        Value result = op(first, second, third, interp);
        interp.evalStack.pushBack(result);
        return ExecutionResult{ExecutionResultType::Success, U""};
    };
}