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
    interp.symTable.createScope();
    while (true) {
        ExecutionResult res = interp.callInterpreter(condCode.arr, false);
        if_unlikely(res.result != ExecutionResultType::Success) {
            interp.symTable.leaveScope();
            return res;
        }
        std::optional<Value> testResult = interp.evalStack.popBack();
        if_unlikely(!testResult.has_value()) {
            interp.symTable.leaveScope();
            return EvalStackUnderflow();
        }
        if_unlikely(testResult.value().type != ValueType::Boolean) {
            interp.symTable.leaveScope();
            return TypeError();
        }
        if (!testResult.value().booleanValue) {
            break;
        }
        res = interp.callInterpreter(loopCode.arr, true);
        if (res.result != ExecutionResultType::Success) {
            if (res.result == ExecutionResultType::Break) {
                interp.symTable.leaveScope();
                return Success();
            } else if (res.result == ExecutionResultType::Continue) {
                continue;
            }
            interp.symTable.leaveScope();
            return res;
        }
    }
    interp.symTable.leaveScope();
    return Success();
}

ExecutionResult forOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(4)) {
        return EvalStackUnderflow();
    }
    Value loopCode = interp.evalStack.popBack().value();
    Value iterCode = interp.evalStack.popBack().value();
    Value condCode = interp.evalStack.popBack().value();
    Value initCode = interp.evalStack.popBack().value();
    if (initCode.type != ValueType::Array ||
        condCode.type != ValueType::Array ||
        iterCode.type != ValueType::Array ||
        loopCode.type != ValueType::Array) {
        return TypeError();
    }
    interp.symTable.createScope();
    ExecutionResult res = interp.callInterpreter(initCode.arr, false);
    if_unlikely(res.result != ExecutionResultType::Success) { return res; }
    while (true) {
        ExecutionResult res = interp.callInterpreter(condCode.arr, false);
        if_unlikely(res.result != ExecutionResultType::Success) { return res; }
        std::optional<Value> testResult = interp.evalStack.popBack();
        if_unlikely(!testResult.has_value()) {
            interp.symTable.leaveScope();
            return EvalStackUnderflow();
        }
        if_unlikely(testResult.value().type != ValueType::Boolean) {
            interp.symTable.leaveScope();
            return TypeError();
        }
        if (!testResult.value().booleanValue) {
            break;
        }
        res = interp.callInterpreter(loopCode.arr, true);
        if (res.result != ExecutionResultType::Success) {
            if (res.result == ExecutionResultType::Break) {
                interp.symTable.leaveScope();
                return Success();
            } else if (res.result != ExecutionResultType::Continue) {
                return res;
            }
        }
        res = interp.callInterpreter(iterCode.arr, false);
        if_unlikely(res.result != ExecutionResultType::Success) {
            interp.symTable.leaveScope();
            return res;
        }
    }
    interp.symTable.leaveScope();
    return Success();
}

ExecutionResult breakOp(Interpreter&) {
    return ExecutionResult{ExecutionResultType::Break, U"Nowhere to break",
                           Value()};
}

ExecutionResult returnOp(Interpreter&) {
    return ExecutionResult{ExecutionResultType::Return, U"Nowhere to return",
                           Value()};
}

ExecutionResult continueOp(Interpreter&) {
    return ExecutionResult{ExecutionResultType::Continue,
                           U"Nowhere to continue", Value()};
}

ExecutionResult scopeCall(Interpreter& interp) {
    std::optional<Value> newTraceOptional = interp.evalStack.popBack();
    if_unlikely(!newTraceOptional) { return EvalStackUnderflow(); }
    Value newTrace = newTraceOptional.value();
    if (newTrace.type == ValueType::Array) {
        ExecutionResult callResult = interp.callInterpreter(newTrace.arr, true);
        if (callResult.result == ExecutionResultType::Return) {
            return Success();
        }
        return callResult;
    } else if (newTrace.type == ValueType::NativeWord) {
        ExecutionResult callResult = newTrace.word(interp);
        if (callResult.result == ExecutionResultType::Return) {
            return Success();
        }
        return callResult;
    }
    return TypeError();
}

ExecutionResult noScopeCall(Interpreter& interp) {
    std::optional<Value> newTraceOptional = interp.evalStack.popBack();
    if_unlikely(!newTraceOptional) { return EvalStackUnderflow(); }
    Value newTrace = newTraceOptional.value();
    if (newTrace.type == ValueType::Array) {
        ExecutionResult callResult =
            interp.callInterpreter(newTrace.arr, false);
        if (callResult.result == ExecutionResultType::Return) {
            return Success();
        }
        return callResult;
    } else if (newTrace.type == ValueType::NativeWord) {
        ExecutionResult callResult = newTrace.word(interp);
        if (callResult.result == ExecutionResultType::Return) {
            return Success();
        }
        return callResult;
    }
    return TypeError();
}

ExecutionResult tryOp(Interpreter& interp) {
    if_unlikely(!interp.evalStack.assertDepth(2)) {
        return EvalStackUnderflow();
    }
    Value argsCount = interp.evalStack.popBack().value();
    Value newTrace = interp.evalStack.popBack().value();
    if_unlikely(newTrace.type != ValueType::Array) { return TypeError(); }
    if_unlikely(argsCount.type != ValueType::Numeric) { return TypeError(); }
    if_unlikely(!interp.evalStack.assertDepth(argsCount.numericValue)) {
        return EvalStackUnderflow();
    }
    size_t cleanSize = interp.evalStack.getStackSize() - argsCount.numericValue;
    size_t oldBarrier = interp.evalStack.getBarrier();
    interp.evalStack.setBarrier(cleanSize);
    ExecutionResult callResult = interp.callInterpreter(newTrace.arr, true);
    Value val;
    val.type = ValueType::Boolean;
    interp.evalStack.setBarrier(oldBarrier);
    if (callResult.result == ExecutionResultType::Success) {
        val.booleanValue = true;
    } else if (callResult.result == ExecutionResultType::Custom) {
        interp.evalStack.resize(cleanSize);
        interp.evalStack.pushBack(callResult.val);
        val.booleanValue = false;
    } else {
        interp.evalStack.resize(cleanSize);
        Value errorMessage;
        errorMessage.type = ValueType::String;
        errorMessage.str = interp.heap.makeStringObject(callResult.error);
        interp.evalStack.pushBack(errorMessage);
        val.booleanValue = false;
    }
    interp.evalStack.pushBack(val);
    return Success();
}

ExecutionResult throwOp(Interpreter& interp) {
    if_unlikely(!interp.evalStack.assertDepth(1)) {
        return EvalStackUnderflow();
    }
    Value val = interp.evalStack.popBack().value();
    return ExecutionResult{ExecutionResultType::Custom, U"", val};
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
    interp.defineNativeWord(U"try", tryOp);
    interp.defineNativeWord(U"throw", throwOp);
}