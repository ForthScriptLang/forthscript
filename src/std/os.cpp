#include <io/fileio.hpp>
#include <io/termio.hpp>
#include <std/os.hpp>

std::string fromUTF32(const std::u32string& s);

ExecutionResult printStrOp(Interpreter& interp) {
    std::optional<Value> top = interp.evalStack.popBack();
    if (!top) {
        return EvalStackUnderflow();
    }
    if (top.value().type != ValueType::String) {
        return TypeError();
    }
    print(top.value().str->get());
    return Success();
}

ExecutionResult quitOp([[maybe_unused]] Interpreter& interp) {
    exit(0);
    // not sure what we are doing here
    return Success();
}

ExecutionResult exitOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(1)) {
        return EvalStackUnderflow();
    }
    Value val = interp.evalStack.popBack().value();
    if (val.type != ValueType::Numeric) {
        return TypeError();
    }
    exit((int)val.numericValue);
    // again, not sure what we are doing here
    return Success();
}

ExecutionResult readFileOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(1)) {
        return EvalStackUnderflow();
    }
    Value filename = interp.evalStack.popBack().value();
    if (filename.type != ValueType::String) {
        return TypeError();
    }
    std::string UTF8filename = fromUTF32(filename.str->get());
    std::optional<std::u32string> result = readFile(UTF8filename.data());
    Value val;
    if (!result.has_value()) {
        val.type = ValueType::Nil;
    } else {
        val.type = ValueType::String;
        val.str = interp.heap.makeStringObject(result.value());
    }
    interp.evalStack.pushBack(val);
    return Success();
}

ExecutionResult readLineOp(Interpreter& interp) {
    std::u32string line = readLine(U"");
    Value result;
    result.type = ValueType::String;
    result.str = interp.heap.makeStringObject(line);
    interp.evalStack.pushBack(result);
    return Success();
}

void addOSModuleNativeWords(Interpreter& interp) {
    interp.defineNativeWord(U"read_line", readLineOp);
    interp.defineNativeWord(U"read_file", readFileOp);
    interp.defineNativeWord(U"print_str", printStrOp);
    interp.defineNativeWord(U"exit", exitOp);
    interp.defineNativeWord(U"quit", quitOp);
}