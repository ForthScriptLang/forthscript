#include <core/interpreter.hpp>
#include <expect.hpp>

Success::Success() {
    result = ExecutionResultType::Success;
    error = U"Success";
}

EvalStackUnderflow::EvalStackUnderflow() {
    result = ExecutionResultType::Error;
    error = U"Evaluation stack underflow";
}

CallStackOverflow::CallStackOverflow() {
    result = ExecutionResultType::Error;
    error = U"Call stack overflow";
}

TypeError::TypeError() {
    result = ExecutionResultType::Error;
    error = U"Type error";
}

Interpreter::Interpreter(size_t maxRecursionDepth)
    : callStack(maxRecursionDepth) {
    callStack.registerRootMarker(heap);
    evalStack.registerRootMarker(heap);
}

void Interpreter::defineNativeWord(const std::u32string& name,
                                   NativeWord word) {
    String* str = heap.makeStringObject(name);
    Value val;
    val.word = word;
    val.type = ValueType::NativeWord;
    symTable.declareVariable(str, val);
    forString = heap.makeStringObject(std::u32string(U"for"));
    whileString = heap.makeStringObject(std::u32string(U"while"));
    breakString = heap.makeStringObject(std::u32string(U"break"));
    returnString = heap.makeStringObject(std::u32string(U"return"));
    callString = heap.makeStringObject(std::u32string(U"!"));
    commaString = heap.makeStringObject(std::u32string(U","));
}

ExecutionResult Interpreter::callInterpreter(Array* code, bool newScope) {
    enum class InterpreterState {
        Interpreting,
        Breaking,
        Returning
    } state = InterpreterState::Interpreting;
    callStack.addArrayCallFrame(code, newScope);
    if (newScope) {
        symTable.createScope();
    }
    while (true) {
        // check for garbage collection
        heap.runGCIfOverThreshold();
        // handling break and return
        if_unlikely(state != InterpreterState::Interpreting) {
            // waiting for "for" or "while"
            while (true) {
                if_unlikely(callStack.frames.empty()) {
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
                    if (callStack.removeTopCallFrame()) {
                        symTable.leaveScope();
                    }
                    continue;
                }
                if (topFrame.code->values.size() == 0) {
                    if (callStack.removeTopCallFrame()) {
                        symTable.leaveScope();
                    }
                    continue;
                }
                const Value& val = topFrame.code->values[topFrame.ip - 1];
                if (val.type != ValueType::Word) {
                    if (callStack.removeTopCallFrame()) {
                        symTable.leaveScope();
                    }
                    continue;
                }
                if (state == InterpreterState::Breaking) {
                    if (val.str == whileString || val.str == forString) {
                        state = InterpreterState::Interpreting;
                        break;
                    }
                } else if (state == InterpreterState::Returning) {
                    if (val.str == callString || val.str == commaString) {
                        state = InterpreterState::Interpreting;
                        break;
                    }
                }
                if (callStack.removeTopCallFrame()) {
                    symTable.leaveScope();
                }
            }
        }
        // nothing on the call stack => interpreter can be exited
        if (callStack.frames.empty()) {
            return Success();
        }
        StackFrame& topFrame = callStack.frames.back();
        // if native subroutine of the stack, aknowledge success
        // and return
        if (topFrame.native) {
            return Success();
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
                if (ins.str == callString || ins.str == commaString) {
                    std::optional<Value> newTraceOptional = evalStack.popBack();
                    if_unlikely(!newTraceOptional) {
                        return EvalStackUnderflow();
                    }
                    Value newTrace = newTraceOptional.value();
                    if_unlikely(newTrace.type != ValueType::Array) {
                        return TypeError();
                    }
                    topFrame.ip++;
                    if_unlikely(!callStack.addArrayCallFrame(
                        newTrace.arr, ins.str == callString)) {
                        return CallStackOverflow();
                    }
                    if (ins.str == callString) {
                        symTable.createScope();
                    }
                    continue;
                } else if (ins.str == returnString || ins.str == breakString) {
                    state = ins.str == returnString
                                ? InterpreterState::Returning
                                : InterpreterState::Breaking;
                    continue;
                } else {
                    Value val = symTable.getVariable(ins.str);
                    topFrame.ip++;
                    if (val.type == ValueType::NativeWord) {
                        if_unlikely(!callStack.addNativeCallFrame()) {
                            return CallStackOverflow();
                        }
                        ExecutionResult result = val.word(*this);
                        callStack.removeTopCallFrame();
                        if_unlikely(result.result !=
                                    ExecutionResultType::Success) {
                            if_likely(result.result ==
                                      ExecutionResultType::Break) {
                                state = InterpreterState::Breaking;
                                continue;
                            }
                            else if_likely(result.result ==
                                           ExecutionResultType::Return) {
                                state = InterpreterState::Returning;
                                continue;
                            }
                            else {
                                return result;
                            }
                        }
                    } else {
                        evalStack.pushBack(val);
                    }
                }
                break;
            case ValueType::WordAssign:
            case ValueType::WordDeclare:
                std::optional<Value> valOptional = evalStack.popBack();
                if_unlikely(!valOptional) { return EvalStackUnderflow(); }
                Value val = valOptional.value();
                if (ins.type == ValueType::WordAssign) {
                    symTable.setVariable(ins.str, val);
                } else {
                    symTable.declareVariable(ins.str, val);
                }
                topFrame.ip++;
                break;
        }
    }
}