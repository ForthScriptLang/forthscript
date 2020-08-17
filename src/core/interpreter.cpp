#include <assert.h>

#include <core/interpreter.hpp>

Interpreter::Interpreter(size_t maxRecursionDepth)
    : callStack(maxRecursionDepth) {
    callStack.registerRootMarker(heap);
    evalStack.registerRootMarker(heap);
    symTable.registerRootMarker(heap);
}

void Interpreter::defineNativeWord(const std::u32string& name,
                                   NativeWord word) {
    Value val;
    val.word = word;
    val.type = ValueType::NativeWord;
    symTable.declareVariable(name, val);
}

ExecutionResult Interpreter::callInterpreter(Array* code,
                                             const std::u32string& name,
                                             bool newScope) {
    callStack.addArrayCallFrame(code, name, newScope);
    if (newScope) {
        symTable.createScope();
    }
    while (true) {
        // nothing on the call stack => interpreter can be exited
        if (callStack.frames.empty()) {
            return ExecutionResult{ExecutionResultType::Success, U""};
        }
        StackFrame& topFrame = callStack.frames.back();
        // if native subroutine of the stack, aknowledge success
        // and return
        if (topFrame.native) {
            return ExecutionResult{ExecutionResultType::Success, U""};
        }
        // if ip is beyond frame size, perform automatic return
        if (topFrame.code->values.size() <= topFrame.ip) {
            if (callStack.removeTopCallFrame()) {
                symTable.leaveScope();
            }
            continue;
        }
        // fetch current instruction
        Value ins = topFrame.code->values[topFrame.ip];
        // execute instruction depending on its type
        switch (ins.type) {
            case ValueType::Nil:
            case ValueType::Boolean:
            case ValueType::Numeric:
            case ValueType::String:
            case ValueType::Placeholder:
            case ValueType::Array:
            case ValueType::NativeWord:
                evalStack.pushBack(ins);
                topFrame.ip++;
                break;
            case ValueType::Word:
                if (ins.str->get() == U"!" || ins.str->get() == U",") {
                    std::optional<Value> newTraceOptional = evalStack.popBack();
                    if (!newTraceOptional) {
                        return ExecutionResult{ExecutionResultType::Error,
                                               U"Evaluation stack underflow"};
                    }
                    Value newTrace = newTraceOptional.value();
                    if (newTrace.type != ValueType::Array) {
                        return ExecutionResult{ExecutionResultType::Error,
                                               U"Type error"};
                    }
                    std::u32string frameName = U"<unknown>";
                    // fancy method for getting func name
                    topFrame.ip++;
                    if (!callStack.addArrayCallFrame(newTrace.arr, frameName,
                                                     ins.str->get() == U"!")) {
                        return ExecutionResult{ExecutionResultType::Error,
                                               U"Call stack overflow"};
                    }
                    if (ins.str->get() == U"!") {
                        symTable.createScope();
                    }
                    continue;
                } else {
                    Value val = symTable.getVariable(ins.str->get());
                    if (val.type == ValueType::NativeWord) {
                        if (!callStack.addNativeCallFrame(ins.str->get())) {
                            return ExecutionResult{ExecutionResultType::Error,
                                                   U"Call stack overflow"};
                        }
                        ExecutionResult result = val.word(*this);
                        if (result.result != ExecutionResultType::Success) {
                            return result;
                        }
                        callStack.removeTopCallFrame();
                    } else {
                        evalStack.pushBack(val);
                    }
                    topFrame.ip++;
                }
                break;
            case ValueType::WordAssign:
            case ValueType::WordDeclare:
                std::optional<Value> valOptional = evalStack.popBack();
                if (!valOptional) {
                    return ExecutionResult{ExecutionResultType::Error,
                                           U"Evaluation stack underflow"};
                }
                Value val = valOptional.value();
                if (ins.type == ValueType::WordAssign) {
                    symTable.setVariable(ins.str->get(), val);
                } else {
                    symTable.declareVariable(ins.str->get(), val);
                }
                topFrame.ip++;
                break;
        }
    }
}