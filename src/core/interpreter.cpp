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

ExecutionResult Interpreter::callInterpreter(Array* code, bool newScope) {
    callStack.addArrayCallFrame(code, U"_start");
    if (newScope) {
        symTable.createScope();
        callStack.addArrayCallFrame(code, U"_start");
    }
    while (true) {
        // nothing on the call stack => interpreter can be exited
        if (callStack.frames.empty()) {
            return ExecutionResult{ExecutionResultType::Success,
                                   ExecutionErrorType::Success};
        }
        StackFrame& topFrame = callStack.frames.back();
        // if native subroutine of the stack, aknowledge success
        // and return
        if (topFrame.native) {
            return ExecutionResult{ExecutionResultType::Success,
                                   ExecutionErrorType::Success};
        }
        // if ip is beyond frame size, perform automatic return
        if (topFrame.code->values.size() <= topFrame.ip) {
            callStack.removeTopCallFrame();
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
                evalStack.stack.push_back(ins);
                topFrame.ip++;
                break;
            case ValueType::Word:
                if (ins.str->str == U"!" || ins.str->str == U",") {
                    if (evalStack.stack.empty()) {
                        return ExecutionResult{
                            ExecutionResultType::Error,
                            ExecutionErrorType::EvalStackUnderflow};
                    }
                    Value& newTrace = evalStack.stack.back();
                    if (newTrace.type != ValueType::Array) {
                        return ExecutionResult{ExecutionResultType::Error,
                                               ExecutionErrorType::TypeError};
                    }
                    topFrame.ip++;
                    if (!callStack.addArrayCallFrame(newTrace.arr,
                                                     U"<unknown>")) {
                        return ExecutionResult{
                            ExecutionResultType::Error,
                            ExecutionErrorType::CallStackOverflow};
                    }
                    if (ins.str->str == U"!") {
                        symTable.createScope();
                    }
                    evalStack.stack.pop_back();
                    continue;
                } else if (nativeWords.find(ins.str->str) !=
                           nativeWords.end()) {
                    if (!callStack.addNativeCallFrame(ins.str->str)) {
                        return ExecutionResult{
                            ExecutionResultType::Error,
                            ExecutionErrorType::CallStackOverflow};
                    }
                    ExecutionResult result = nativeWords[ins.str->str](*this);
                    if (result.result != ExecutionResultType::Success) {
                        return result;
                    }
                    callStack.removeTopCallFrame();
                    topFrame.ip++;
                } else {
                    evalStack.stack.push_back(
                        symTable.getVariable(ins.str->str));
                    topFrame.ip++;
                }
                break;
            case ValueType::WordAssign:
            case ValueType::WordDeclare:
                if (evalStack.stack.empty()) {
                    return ExecutionResult{
                        ExecutionResultType::Error,
                        ExecutionErrorType::EvalStackUnderflow};
                }
                Value val = evalStack.stack.back();
                evalStack.stack.pop_back();
                if (ins.type == ValueType::WordAssign) {
                    symTable.setVariable(ins.str->str, val);
                } else {
                    symTable.declareVariable(ins.str->str, val);
                }
                topFrame.ip++;
                break;
        }
    }
}