#include <codecvt>
#include <core/interpreter.hpp>
#include <io/fileio.hpp>
#include <io/parser.hpp>
#include <io/prettyprint.hpp>
#include <io/termio.hpp>
#include <iostream>
#include <locale>
#include <prelude/repl.hpp>
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
    print(U" : ");
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

int hostRepl() {
    Interpreter interp(1024);
    // initStd requires scope for adding native words
    interp.symTable.createScope();
    initStd(interp);
    initREPL(interp);
    ParseResult result = parse(getPreludeREPLSource(), interp);
    if (result.isError()) {
        std::cerr << "The following parser error was encountered while parsing "
                     "REPL prelude. This is the bug in Forthscript "
                     "interpreter. Please report it";
        reportSyntaxError(result);
        interp.heap.collectGarbage();
        return -1;
    }
    ExecutionResult res = interp.callInterpreter(result.code, false);
    if (res.result != ExecutionResultType::Success) {
        std::cerr << "Prelude has not handled the following error:\n";
        reportRuntimeError(res, interp);
        interp.evalStack.clear();
        interp.heap.collectGarbage();
        return -1;
    }
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        return hostRepl();
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
