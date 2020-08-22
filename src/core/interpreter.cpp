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
    : symTable(maxRecursionDepth) {
    evalStack.registerRootMarker(heap);

    callString = heap.makeStringObject(std::u32string(U"!"));
    commaString = heap.makeStringObject(std::u32string(U","));
}

void Interpreter::defineNativeWord(const std::u32string& name,
                                   NativeWord word) {
    String* str = heap.makeStringObject(name);
    stringsToSymbols[str] = word;
    symbolsToStrings[word] = str;
}

NativeWord Interpreter::queryNativeWord(String* str) {
    if (stringsToSymbols.find(str) == stringsToSymbols.end()) {
        return nullptr;
    }
    return stringsToSymbols[str];
}

ExecutionResult Interpreter::callInterpreter(Array* code, bool newScope) {
    if (newScope) {
        symTable.createScope();
    }
    for (size_t i = 0; i < code->values.size(); ++i) {
        Value ins = code->values[i];
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
                ExecutionResult result = ins.word(*this);
                if (result.result != ExecutionResultType::Success) {
                    return result;
                }
                // check for garbage collection
                heap.runGCIfOverThreshold();
            } break;
        }
    }
    return Success();
}