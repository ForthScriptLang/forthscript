#include <std/controlflow.hpp>

ExecutionResult ifElseOp(Interpreter& interp) {
    if (interp.evalStack.stack.size() < 3) {
        return ExecutionResult{ExecutionResultType::Error,
                               U"Evaluation stack underflow"};
    }
    Value ifCodeVal = interp.evalStack.stack.back();
    interp.evalStack.stack.pop_back();
    Value elseCodeVal = interp.evalStack.stack.back();
    interp.evalStack.stack.pop_back();
    Value condition = interp.evalStack.stack.back();
    interp.evalStack.stack.pop_back();
    if (elseCodeVal.type != ValueType::Array ||
        ifCodeVal.type != ValueType::Array ||
        condition.type != ValueType::Boolean) {
        return ExecutionResult{ExecutionResultType::Error, U"Type error"};
    }
    if (condition.booleanValue) {
        ExecutionResult res = interp.callInterpreter(ifCodeVal.arr, true);
        if (res.result != ExecutionResultType::Success) {
            return res;
        }
    } else {
        ExecutionResult res = interp.callInterpreter(elseCodeVal.arr, true);
        if (res.result != ExecutionResultType::Success) {
            return res;
        }
    }
    return ExecutionResult{ExecutionResultType::Success, U""};
}

ExecutionResult ifOp(Interpreter& interp) {
    if (interp.evalStack.stack.size() < 2) {
        return ExecutionResult{ExecutionResultType::Error,
                               U"Evaluation stack underflow"};
    }
    Value ifCodeVal = interp.evalStack.stack.back();
    interp.evalStack.stack.pop_back();
    Value condition = interp.evalStack.stack.back();
    interp.evalStack.stack.pop_back();
    if (ifCodeVal.type != ValueType::Array ||
        condition.type != ValueType::Boolean) {
        return ExecutionResult{ExecutionResultType::Error,
                               U"Evaluation stack underflow"};
    }
    if (condition.booleanValue) {
        ExecutionResult res = interp.callInterpreter(ifCodeVal.arr, true);
        if (res.result != ExecutionResultType::Success) {
            return res;
        }
    }
    return ExecutionResult{ExecutionResultType::Success, U""};
}

ExecutionResult whileOp(Interpreter& interp) {
    if (interp.evalStack.stack.size() < 2) {
        return ExecutionResult{ExecutionResultType::Error,
                               U"Evaluation stack underflow"};
    }
    Value loopCode = interp.evalStack.stack.back();
    interp.evalStack.stack.pop_back();
    Value condCode = interp.evalStack.stack.back();
    interp.evalStack.stack.pop_back();
    if (condCode.type != ValueType::Array ||
        loopCode.type != ValueType::Array) {
        return ExecutionResult{ExecutionResultType::Error, U"Type error"};
    }
    while (true) {
        ExecutionResult res = interp.callInterpreter(condCode.arr, true);
        if (res.result != ExecutionResultType::Success) {
            return res;
        }
        if (interp.evalStack.stack.empty()) {
            return ExecutionResult{ExecutionResultType::Error,
                                   U"Evaluation stack underflow"};
        }
        Value val = interp.evalStack.stack.back();
        interp.evalStack.stack.pop_back();
        if (val.type != ValueType::Boolean) {
            ExecutionResult{ExecutionResultType::Error, U"Type error"};
        }
        if (!val.booleanValue) {
            break;
        }
        res = interp.callInterpreter(loopCode.arr, true);
        if (res.result != ExecutionResultType::Success) {
            return res;
        }
    }
    return ExecutionResult{ExecutionResultType::Success, U""};
}

ExecutionResult forOp(Interpreter& interp) {
    if (interp.evalStack.stack.size() < 2) {
        return ExecutionResult{ExecutionResultType::Error,
                               U"Evaluation stack underflow"};
    }
    Value loopCode = interp.evalStack.stack.back();
    interp.evalStack.stack.pop_back();
    Value iterCode = interp.evalStack.stack.back();
    interp.evalStack.stack.pop_back();
    Value condCode = interp.evalStack.stack.back();
    interp.evalStack.stack.pop_back();
    if (condCode.type != ValueType::Array ||
        iterCode.type != ValueType::Array ||
        loopCode.type != ValueType::Array) {
        return ExecutionResult{ExecutionResultType::Error, U"Type error"};
    }
    while (true) {
        ExecutionResult res = interp.callInterpreter(condCode.arr, true);
        if (res.result != ExecutionResultType::Success) {
            return res;
        }
        if (interp.evalStack.stack.empty()) {
            return ExecutionResult{ExecutionResultType::Error,
                                   U"Evaluation stack underflow"};
        }
        Value val = interp.evalStack.stack.back();
        interp.evalStack.stack.pop_back();
        if (val.type != ValueType::Boolean) {
            ExecutionResult{ExecutionResultType::Error, U"Type error"};
        }
        if (!val.booleanValue) {
            break;
        }
        res = interp.callInterpreter(loopCode.arr, true);
        if (res.result != ExecutionResultType::Success) {
            return res;
        }
        res = interp.callInterpreter(iterCode.arr, true);
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
}