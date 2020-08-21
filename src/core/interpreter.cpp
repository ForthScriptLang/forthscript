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
    : callStack(maxRecursionDepth), symTable(maxRecursionDepth) {
    callStack.registerRootMarker(heap);
    evalStack.registerRootMarker(heap);

    callString = heap.makeStringObject(std::u32string(U"!"));
    commaString = heap.makeStringObject(std::u32string(U","));
}

bool Interpreter::cleanFramesToLastNative() {
    while (!callStack.frames.empty()) {
        if (callStack.frames.back().native) {
            return true;
        }
        callStack.frames.pop_back();
    }
    return false;
}

void Interpreter::defineNativeWord(const std::u32string& name,
                                   NativeWord word) {
    String* str = heap.makeStringObject(name);
    Value val;
    val.word = word;
    val.type = ValueType::NativeWord;
    symTable.declareVariable(str, val);
    symbols[word] = str;
}

NativeWord Interpreter::queryNativeWord(String* str) {
    Value val = symTable.getVariable(str);
    if (val.type != ValueType::NativeWord) {
        return nullptr;
    }
    return val.word;
}

ExecutionResult Interpreter::callInterpreter(Array* code, bool newScope) {
    callStack.addArrayCallFrame(code, newScope);
    if (newScope) {
        symTable.createScope();
    }
    while (true) {
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
        Value ins = topFrame.code->values[topFrame.ip++];
        // execute instruction depending on its type
        switch (ins.type) {
            case ValueType::Nil:
            case ValueType::Boolean:
            case ValueType::Numeric:
            case ValueType::String:
            case ValueType::Placeholder:
            case ValueType::Array:
                evalStack.pushBack(ins);
                break;
            case ValueType::Word: {
                Value val = symTable.getVariable(ins.str);
                evalStack.pushBack(val);
            } break;
            case ValueType::WordAssign:
            case ValueType::WordDeclare: {
                std::optional<Value> valOptional = evalStack.popBack();
                if_unlikely(!valOptional) { return EvalStackUnderflow(); }
                Value val = valOptional.value();
                if (ins.type == ValueType::WordAssign) {
                    symTable.setVariable(ins.str, val);
                } else {
                    symTable.declareVariable(ins.str, val);
                }
            } break;
            case ValueType::NativeWord: {
                if_unlikely(!callStack.addNativeCallFrame()) {
                    return CallStackOverflow();
                }
                ExecutionResult result = ins.word(*this);
                if (callStack.frames.back().native) {
                    callStack.removeTopCallFrame();
                }
                if (result.result != ExecutionResultType::Success) {
                    cleanFramesToLastNative();
                    return result;
                }
                // check for garbage collection
                heap.runGCIfOverThreshold();
            } break;
        }
    }
}