#include <expect.hpp>
#include <std/controlflow.hpp>

ExecutionResult ifElseOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(3)) {
        return EvalStackUnderflow();
    }

    Value elseCodeVal = interp.evalStack.popBack().value();
    Value ifCodeVal = interp.evalStack.popBack().value();
    Value condition = interp.evalStack.popBack().value();
    if_unlikely(elseCodeVal.type != ValueType::Array ||
                ifCodeVal.type != ValueType::Array ||
                condition.type != ValueType::Boolean) {
        return TypeError();
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
    return Success();
}

ExecutionResult ifOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(2)) {
        return EvalStackUnderflow();
    }

    Value ifCodeVal = interp.evalStack.popBack().value();
    Value condition = interp.evalStack.popBack().value();
    if_unlikely(ifCodeVal.type != ValueType::Array ||
                condition.type != ValueType::Boolean) {
        return TypeError();
    }
    if (condition.booleanValue) {
        ExecutionResult res = interp.callInterpreter(ifCodeVal.arr, true);
        if (res.result != ExecutionResultType::Success) {
            return res;
        }
    }
    return Success();
}

ExecutionResult whileOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(2)) {
        return EvalStackUnderflow();
    }

    Value loopCode = interp.evalStack.popBack().value();
    Value condCode = interp.evalStack.popBack().value();
    if_unlikely(condCode.type != ValueType::Array ||
                loopCode.type != ValueType::Array) {
        return TypeError();
    }
    while (true) {
        ExecutionResult res = interp.callInterpreter(condCode.arr, true);
        if_unlikely(res.result != ExecutionResultType::Success) { return res; }
        std::optional<Value> testResult = interp.evalStack.popBack();
        if_unlikely(testResult.value().type != ValueType::Boolean) {
            TypeError();
        }
        if (!testResult.value().booleanValue) {
            break;
        }
        res = interp.callInterpreter(loopCode.arr, true);
        if (res.result != ExecutionResultType::Success) {
            if (res.result == ExecutionResultType::Break) {
                return Success();
            } else if (res.result == ExecutionResultType::Continue) {
                continue;
            }
            return res;
        }
    }
    return Success();
}

ExecutionResult forOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(3)) {
        return EvalStackUnderflow();
    }
    Value loopCode = interp.evalStack.popBack().value();
    Value iterCode = interp.evalStack.popBack().value();
    Value condCode = interp.evalStack.popBack().value();
    if (condCode.type != ValueType::Array ||
        iterCode.type != ValueType::Array ||
        loopCode.type != ValueType::Array) {
        return TypeError();
    }
    while (true) {
        ExecutionResult res = interp.callInterpreter(condCode.arr, true);
        if_unlikely(res.result != ExecutionResultType::Success) { return res; }
        std::optional<Value> testResult = interp.evalStack.popBack();
        if_unlikely(testResult.value().type != ValueType::Boolean) {
            TypeError();
        }
        if (!testResult.value().booleanValue) {
            break;
        }
        res = interp.callInterpreter(loopCode.arr, true);
        if (res.result != ExecutionResultType::Success) {
            if (res.result == ExecutionResultType::Break) {
                return Success();
            } else if (res.result != ExecutionResultType::Continue) {
                return res;
            }
        }
        res = interp.callInterpreter(iterCode.arr, true);
        if_unlikely(res.result != ExecutionResultType::Success) { return res; }
    }
    return Success();
}

ExecutionResult breakOp(Interpreter&) {
    return ExecutionResult{ExecutionResultType::Break, U"Nowhere to break"};
}

ExecutionResult returnOp(Interpreter&) {
    return ExecutionResult{ExecutionResultType::Return, U"Nowhere to return"};
}

ExecutionResult continueOp(Interpreter&) {
    return ExecutionResult{ExecutionResultType::Continue,
                           U"Nowhere to continue"};
}

ExecutionResult scopeCall(Interpreter& interp) {
    std::optional<Value> newTraceOptional = interp.evalStack.popBack();
    if_unlikely(!newTraceOptional) { return EvalStackUnderflow(); }
    Value newTrace = newTraceOptional.value();
    if_unlikely(newTrace.type != ValueType::Array) { return TypeError(); }
    ExecutionResult callResult = interp.callInterpreter(newTrace.arr, true);
    if (callResult.result == ExecutionResultType::Return) {
        return Success();
    }
    return callResult;
}

ExecutionResult noScopeCall(Interpreter& interp) {
    std::optional<Value> newTraceOptional = interp.evalStack.popBack();
    if_unlikely(!newTraceOptional) { return EvalStackUnderflow(); }
    Value newTrace = newTraceOptional.value();
    if_unlikely(newTrace.type != ValueType::Array) { return TypeError(); }
    ExecutionResult callResult = interp.callInterpreter(newTrace.arr, false);
    if (callResult.result == ExecutionResultType::Return) {
        return Success();
    }
    return callResult;
}

void addControlFlowNativeWords(Interpreter& interp) {
    interp.defineNativeWord(U"while", whileOp);
    interp.defineNativeWord(U"if_else", ifElseOp);
    interp.defineNativeWord(U"if", ifOp);
    interp.defineNativeWord(U"for", forOp);
    interp.defineNativeWord(U"break", breakOp);
    interp.defineNativeWord(U"return", returnOp);
    interp.defineNativeWord(U"continue", continueOp);
    interp.defineNativeWord(U"!", scopeCall);
    interp.defineNativeWord(U",", noScopeCall);
}