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
    enum class InterpreterState {
        Interpreting,
        Breaking,
        Returning
    } state = InterpreterState::Interpreting;
    callStack.addArrayCallFrame(code, name, newScope);
    if (newScope) {
        symTable.createScope();
    }
    while (true) {
        // handling break and return
        if (state != InterpreterState::Interpreting) {
            // waiting for "for" or "while"
            while (true) {
                if (callStack.frames.empty()) {
                    return ExecutionResult{ExecutionResultType::Error,
                                           U"Nowhere to break"};
                }
                StackFrame& topFrame = callStack.frames.back();
                if (topFrame.native) {
                    return ExecutionResult{state == InterpreterState::Breaking
                                               ? ExecutionResultType::Break
                                               : ExecutionResultType::Return,
                                           U""};
                }
                if (topFrame.ip == 0) {
                    callStack.frames.pop_back();
                    continue;
                }
                if (topFrame.code->values.size() == 0) {
                    callStack.frames.pop_back();
                    continue;
                }
                const Value& val = topFrame.code->values[topFrame.ip - 1];
                if (val.type != ValueType::Word) {
                    callStack.frames.pop_back();
                    continue;
                }
                if (state == InterpreterState::Breaking) {
                    if (val.str->str == U"while" || val.str->str == U"for") {
                        state = InterpreterState::Interpreting;
                        break;
                    }
                } else if (state == InterpreterState::Returning) {
                    if (val.str->str == U"!" || val.str->str == U",") {
                        state = InterpreterState::Interpreting;
                        break;
                    }
                }
            }
        }
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
                evalStack.stack.push_back(ins);
                topFrame.ip++;
                break;
            case ValueType::Word:
                if (ins.str->str == U"!" || ins.str->str == U",") {
                    if (evalStack.stack.empty()) {
                        return ExecutionResult{ExecutionResultType::Error,
                                               U"Evaluation stack underflow"};
                    }
                    Value& newTrace = evalStack.stack.back();
                    if (newTrace.type != ValueType::Array) {
                        return ExecutionResult{ExecutionResultType::Error,
                                               U"Type error"};
                    }
                    std::u32string frameName = U"<unknown>";
                    // fancy method for getting func name
                    if (topFrame.ip != 0 &&
                        topFrame.code->values[topFrame.ip - 1].type ==
                            ValueType::Word &&
                        nativeWords.find(
                            topFrame.code->values[topFrame.ip - 1].str->str) ==
                            nativeWords.end()) {
                        frameName =
                            topFrame.code->values[topFrame.ip - 1].str->str;
                    }
                    topFrame.ip++;
                    if (!callStack.addArrayCallFrame(newTrace.arr, frameName,
                                                     ins.str->str == U"!")) {
                        return ExecutionResult{ExecutionResultType::Error,
                                               U"Call stack overflow"};
                    }
                    if (ins.str->str == U"!") {
                        symTable.createScope();
                    }
                    evalStack.stack.pop_back();
                    continue;
                } else if (ins.str->str == U"return" ||
                           ins.str->str == U"break") {
                    state = ins.str->str == U"return"
                                ? InterpreterState::Returning
                                : InterpreterState::Breaking;
                } else if (nativeWords.find(ins.str->str) !=
                           nativeWords.end()) {
                    if (!callStack.addNativeCallFrame(ins.str->str)) {
                        return ExecutionResult{ExecutionResultType::Error,
                                               U"Call stack overflow"};
                    }
                    topFrame.ip++;
                    ExecutionResult result = nativeWords[ins.str->str](*this);
                    if (result.result != ExecutionResultType::Success) {
                        if (result.result == ExecutionResultType::Break) {
                            state = InterpreterState::Breaking;
                            callStack.removeTopCallFrame();
                            continue;
                        } else if (result.result ==
                                   ExecutionResultType::Return) {
                            state = InterpreterState::Returning;
                            callStack.removeTopCallFrame();
                            continue;
                        }
                        return result;
                    }
                    callStack.removeTopCallFrame();
                } else {
                    evalStack.stack.push_back(
                        symTable.getVariable(ins.str->str));
                    topFrame.ip++;
                }
                break;
            case ValueType::WordAssign:
            case ValueType::WordDeclare:
                if (evalStack.stack.empty()) {
                    return ExecutionResult{ExecutionResultType::Error,
                                           U"Evaluation stack underflow"};
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