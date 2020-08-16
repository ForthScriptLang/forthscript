#include <io/termio.hpp>
#include <std/os.hpp>

ExecutionResult printStrOp(Interpreter& interp) {
    std::optional<Value> top = interp.evalStack.popBack();
    if (!top) {
        return ExecutionResult{ExecutionResultType::Error,
                               U"Evaluation stack underflow"};
    }
    if (top.value().type != ValueType::String) {
        return ExecutionResult{ExecutionResultType::Error, U"Type error"};
    }
    print(top.value().str->get());
    return ExecutionResult{ExecutionResultType::Success, U""};
}

void addOSModuleNativeWords(Interpreter& interp) {
    interp.defineNativeWord(U"print_str", printStrOp);
}