#include <std/controlflow.hpp>

ExecutionResult ifElseOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(3)) {
        return ExecutionResult{ExecutionResultType::Error,
                               U"Evaluation stack underflow"};
    }

    Value elseCodeVal = interp.evalStack.popBack().value();
    Value ifCodeVal = interp.evalStack.popBack().value();
    Value condition = interp.evalStack.popBack().value();
    if (elseCodeVal.type != ValueType::Array ||
        ifCodeVal.type != ValueType::Array ||
        condition.type != ValueType::Boolean) {
        return ExecutionResult{ExecutionResultType::Error, U"Type error"};
    }
    if (condition.booleanValue) {
        ExecutionResult res =
            interp.callInterpreter(ifCodeVal.arr, U"if_body", true);
        if (res.result != ExecutionResultType::Success) {
            return res;
        }
    } else {
        ExecutionResult res =
            interp.callInterpreter(elseCodeVal.arr, U"else_body", true);
        if (res.result != ExecutionResultType::Success) {
            return res;
        }
    }
    return ExecutionResult{ExecutionResultType::Success, U""};
}

ExecutionResult ifOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(2)) {
        return ExecutionResult{ExecutionResultType::Error,
                               U"Evaluation stack underflow"};
    }

    Value ifCodeVal = interp.evalStack.popBack().value();
    Value condition = interp.evalStack.popBack().value();
    if (ifCodeVal.type != ValueType::Array ||
        condition.type != ValueType::Boolean) {
        return ExecutionResult{ExecutionResultType::Error,
                               U"Evaluation stack underflow"};
    }
    if (condition.booleanValue) {
        ExecutionResult res = interp.callInterpreter(ifCodeVal.arr, U"", true);
        if (res.result != ExecutionResultType::Success) {
            return res;
        }
    }
    return ExecutionResult{ExecutionResultType::Success, U""};
}

ExecutionResult whileOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(2)) {
        return ExecutionResult{ExecutionResultType::Error,
                               U"Evaluation stack underflow"};
    }

    Value loopCode = interp.evalStack.popBack().value();
    Value condCode = interp.evalStack.popBack().value();
    if (condCode.type != ValueType::Array ||
        loopCode.type != ValueType::Array) {
        return ExecutionResult{ExecutionResultType::Error, U"Type error"};
    }
    while (true) {
        ExecutionResult res =
            interp.callInterpreter(condCode.arr, U"condition", true);
        if (res.result != ExecutionResultType::Success) {
            return res;
        }
        std::optional<Value> testResult = interp.evalStack.popBack();
        if (testResult.value().type != ValueType::Boolean) {
            ExecutionResult{ExecutionResultType::Error, U"Type error"};
        }
        if (!testResult.value().booleanValue) {
            break;
        }
        res = interp.callInterpreter(loopCode.arr, U"loop_body", true);
        if (res.result != ExecutionResultType::Success) {
            return res;
        }
    }
    return ExecutionResult{ExecutionResultType::Success, U""};
}

ExecutionResult forOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(3)) {
        return ExecutionResult{ExecutionResultType::Error,
                               U"Evaluation stack underflow"};
    }
    Value loopCode = interp.evalStack.popBack().value();
    Value iterCode = interp.evalStack.popBack().value();
    Value condCode = interp.evalStack.popBack().value();
    if (condCode.type != ValueType::Array ||
        iterCode.type != ValueType::Array ||
        loopCode.type != ValueType::Array) {
        return ExecutionResult{ExecutionResultType::Error, U"Type error"};
    }
    while (true) {
        ExecutionResult res =
            interp.callInterpreter(condCode.arr, U"condition", true);
        if (res.result != ExecutionResultType::Success) {
            return res;
        }
        std::optional<Value> testResult = interp.evalStack.popBack();
        if (testResult.value().type != ValueType::Boolean) {
            ExecutionResult{ExecutionResultType::Error, U"Type error"};
        }
        if (!testResult.value().booleanValue) {
            break;
        }
        res = interp.callInterpreter(loopCode.arr, U"loop_body", true);
        if (res.result != ExecutionResultType::Success) {
            return res;
        }
        res = interp.callInterpreter(iterCode.arr, U"next_body", true);
        if (res.result != ExecutionResultType::Success) {
            return res;
        }
    }
    return ExecutionResult{ExecutionResultType::Success, U""};
}

void addControlFlowNativeWords(Interpreter& interp) {
    interp.defineNativeWord(U"while", whileOp);
    interp.defineNativeWord(U"if_else", ifElseOp);
    interp.defineNativeWord(U"if", ifOp);
    interp.defineNativeWord(U"for", forOp);
}