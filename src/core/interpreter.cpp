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
    nativeWords[name] = word;
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
                    if (topFrame.ip != 0 &&
                        topFrame.code->values[topFrame.ip - 1].type ==
                            ValueType::Word &&
                        nativeWords.find(topFrame.code->values[topFrame.ip - 1]
                                             .str->get()) ==
                            nativeWords.end()) {
                        frameName =
                            topFrame.code->values[topFrame.ip - 1].str->get();
                    }
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
                } else if (nativeWords.find(ins.str->get()) !=
                           nativeWords.end()) {
                    if (!callStack.addNativeCallFrame(ins.str->get())) {
                        return ExecutionResult{ExecutionResultType::Error,
                                               U"Call stack overflow"};
                    }
                    ExecutionResult result = nativeWords[ins.str->get()](*this);
                    if (result.result != ExecutionResultType::Success) {
                        return result;
                    }
                    callStack.removeTopCallFrame();
                    topFrame.ip++;
                } else {
                    evalStack.pushBack(symTable.getVariable(ins.str->get()));
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