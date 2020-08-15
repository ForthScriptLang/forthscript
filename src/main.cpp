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
            print(U"Meh, error\n");
            interp.heap.collectGarbage();
            continue;
        }
        ExecutionResult res = interp.callInterpreter(result.code, false);
        if (res.result != ExecutionResultType::Success) {
            print(U"Meh, runtime error\n");
        }
    }
}