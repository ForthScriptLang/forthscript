#include <io/fileio.hpp>
#include <io/processes.hpp>
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

ExecutionResult printlnStrOp(Interpreter& interp) {
    std::optional<Value> top = interp.evalStack.popBack();
    if (!top) {
        return EvalStackUnderflow();
    }
    if (top.value().type != ValueType::String) {
        return TypeError();
    }
    print(top.value().str->get());
    print(U"\n");
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

ExecutionResult writeFileOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(2)) {
        return EvalStackUnderflow();
    }
    Value filename = interp.evalStack.popBack().value();
    Value contents = interp.evalStack.popBack().value();
    if (filename.type != ValueType::String ||
        contents.type != ValueType::String) {
        return TypeError();
    }
    bool result = writeFile(filename.str->get(), contents.str->get());
    Value valResult;
    valResult.type = ValueType::Boolean;
    valResult.booleanValue = result;
    interp.evalStack.pushBack(valResult);
    return Success();
}

ExecutionResult execOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(2)) {
        return EvalStackUnderflow();
    }
    Value cmd = interp.evalStack.popBack().value();
    Value in = interp.evalStack.popBack().value();
    if (cmd.type != ValueType::String || in.type != ValueType::String) {
        return TypeError();
    }
    ProcessInvokationRequest request;
    request.name = cmd.str->get();
    request.in = in.str->get();
    ProcessInvokationResponce response = executeProcess(request);
    Value booleanResult, outResult, codeResult;
    booleanResult.type = ValueType::Boolean;
    outResult.type = ValueType::String;
    codeResult.type = ValueType::Numeric;
    if (response.error) {
        booleanResult.booleanValue = false;
        interp.evalStack.pushBack(booleanResult);
    } else {
        outResult.str = interp.heap.makeStringObject(response.out);
        booleanResult.booleanValue = true;
        codeResult.numericValue = response.errorCode;
        interp.evalStack.pushBack(outResult);
        interp.evalStack.pushBack(codeResult);
        interp.evalStack.pushBack(booleanResult);
    }
    return Success();
}

void addOSModuleNativeWords(Interpreter& interp) {
    interp.defineNativeWord(U"readln", readLineOp);
    interp.defineNativeWord(U"readfile", readFileOp);
    interp.defineNativeWord(U"writefile", writeFileOp);
    interp.defineNativeWord(U"writeln", printlnStrOp);
    interp.defineNativeWord(U"write", printStrOp);
    interp.defineNativeWord(U"exec", execOp);
    interp.defineNativeWord(U"exit", exitOp);
    interp.defineNativeWord(U"quit", quitOp);
}