#include <codecvt>
#include <core/interpreter.hpp>
#include <io/fileio.hpp>
#include <io/parser.hpp>
#include <io/prettyprint.hpp>
#include <io/termio.hpp>
#include <iostream>
#include <locale>
#include <std/arith.hpp>
#include <std/arrays.hpp>
#include <std/boolean.hpp>
#include <std/comparisons.hpp>
#include <std/controlflow.hpp>
#include <std/indexing.hpp>
#include <std/os.hpp>
#include <std/stack.hpp>
#include <std/strings.hpp>
#include <std/templates.hpp>
#include <std/typing.hpp>

void initStd(Interpreter& interp) {
    addArithNativeWords(interp);
    addComparisonsNativeWords(interp);
    addStackManipNativeWords(interp);
    addControlFlowNativeWords(interp);
    addOSModuleNativeWords(interp);
    addStringManipulationNativeWords(interp);
    addIndexingNativeWords(interp);
    addTemplatesNativeWords(interp);
    addArrayManipulationNativeWords(interp);
    addTypingNativeWords(interp);
    addBooleanNativeWords(interp);
}

void reportSyntaxError(ParseResult res) {
    if (res.status == ParseResult::Status::LexerError) {
        print(U"Lexer error at position ");
    } else {
        print(U"Parser error at position ");
    }
    std::cout << res.errorPos;
    print(U": ");
    print(res.description);
    print(U"\n");
}

void reportRuntimeError(ExecutionResult res, Interpreter& interp) {
    print(U"Runtime error: ");
    if (res.result == ExecutionResultType::Custom) {
        print(prettyprint(res.val, interp));
    } else {
        print(std::u32string(res.error));
    }
    print(U"\n");
}

void hostRepl() {
    Interpreter interp(1024);
    // initStd requires scope for adding native words
    interp.symTable.createScope();
    initStd(interp);
    initREPL(interp);
    while (true) {
        std::u32string prompt;
        prompt.append(U"[");
        for (size_t i = 0; i < interp.evalStack.getStack().size(); ++i) {
            prompt.append(prettyprint(interp.evalStack.getStack()[i], interp));
            if (i != interp.evalStack.getStack().size() - 1) {
                prompt.append(U" ");
            }
        }
        prompt.append(U"]# ");
        std::u32string s = readLine(prompt);
        ParseResult result = parse(s, interp);
        if (result.isError()) {
            reportSyntaxError(result);
            interp.evalStack.clear();
            interp.heap.collectGarbage();
            continue;
        }
        ExecutionResult res = interp.callInterpreter(result.code, false);
        if (res.result != ExecutionResultType::Success) {
            reportRuntimeError(res, interp);
            interp.evalStack.clear();
            interp.heap.collectGarbage();
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        hostRepl();
        return 0;
    }
    char* filename = argv[1];
    Interpreter interp(1024);
    // initStd requires scope for adding native words
    interp.symTable.createScope();
    initStd(interp);
    std::optional<std::u32string> source = readFile(filename);
    if (!source.has_value()) {
        std::cout << "Can't open file " << filename << std::endl;
        return 0;
    }
    ParseResult result = parse(source.value(), interp);
    if (result.isError()) {
        reportSyntaxError(result);
        interp.heap.collectGarbage();
        return -1;
    }
    ExecutionResult res = interp.callInterpreter(result.code, false);
    if (res.result != ExecutionResultType::Success) {
        reportRuntimeError(res, interp);
        interp.evalStack.clear();
        interp.heap.collectGarbage();
        return -1;
    }
    return 0;
}