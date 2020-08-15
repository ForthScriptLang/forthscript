#include <codecvt>
#include <core/interpreter.hpp>
#include <io/parser.hpp>
#include <io/prettyprint.hpp>
#include <io/termio.hpp>
#include <iostream>
#include <locale>
#include <std/arith.hpp>
#include <std/comparisons.hpp>
#include <std/controlflow.hpp>
#include <std/stack.hpp>

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
    print(res.error);
    print(U"\n");
    for (size_t i = 0; i < interp.callStack.frames.size(); ++i) {
        print(U"at <");
        print(interp.callStack.frames[i].name);
        print(U">\n");
    }
}

int main() {
    Interpreter interp(1024);
    // initialize standard library
    addArithNativeWords(interp);
    addComparisonsNativeWords(interp);
    addStackManipNativeWords(interp);
    addControlFlowNativeWords(interp);

    interp.symTable.createScope();
    while (true) {
        print(U"[");
        for (size_t i = 0; i < interp.evalStack.stack.size(); ++i) {
            print(prettyprint(interp.evalStack.stack[i]));
            if (i != interp.evalStack.stack.size() - 1) {
                print(U" ");
            }
        }
        print(U"]# ");
        std::u32string s = readLine();
        ParseResult result = parse(s, interp.heap);
        if (result.isError()) {
            reportSyntaxError(result);
            interp.heap.collectGarbage();
            continue;
        }
        ExecutionResult res = interp.callInterpreter(result.code, false);
        if (res.result != ExecutionResultType::Success) {
            reportRuntimeError(res, interp);
            interp.evalStack.stack.clear();
            interp.heap.collectGarbage();
        }
    }
}