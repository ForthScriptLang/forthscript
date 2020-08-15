#include <io/termio.hpp>
#include <std/os.hpp>

ExecutionResult printStrOp(Interpreter& interp) {
    if (interp.evalStack.stack.empty()) {
        return ExecutionResult{ExecutionResultType::Error,
                               U"Evaluation stack underflow"};
    }
    Value top = interp.evalStack.stack.back();
    interp.evalStack.stack.pop_back();
    if (top.type != ValueType::String) {
        return ExecutionResult{ExecutionResultType::Error, U"Type error"};
    }
    print(top.str->str);
    return ExecutionResult{ExecutionResultType::Success, U""};
}

void addOSModuleNativeWords(Interpreter& interp) {
    interp.defineNativeWord(U"print_str", printStrOp);
}