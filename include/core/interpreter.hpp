#pragma once

#include <core/callstack.hpp>
#include <core/evalstack.hpp>
#include <core/heap.hpp>
#include <core/symtable.hpp>
#include <core/values.hpp>
#include <functional>
#include <string>
#include <unordered_map>

enum class ExecutionResultType { Error, Success, Return, Break, Continue };

struct ExecutionResult {
    ExecutionResultType result = ExecutionResultType::Success;
    const char32_t* error = U"";
};

struct Success : public ExecutionResult {
    Success();
};

struct EvalStackUnderflow : public ExecutionResult {
    EvalStackUnderflow();
};

struct CallStackOverflow : public ExecutionResult {
    CallStackOverflow();
};

struct TypeError : public ExecutionResult {
    TypeError();
};

struct Interpreter {
    CallStack callStack;
    EvaluationStack evalStack;
    Heap heap;
    SymbolTable symTable;
    String *breakString, *returnString, *callString, *commaString, *forString,
        *whileString;

    Interpreter(size_t maxRecursionDepth);
    void defineNativeWord(const std::u32string& str, NativeWord word);
    ExecutionResult callInterpreter(Array* code, bool newScope);

    // returns true if there was a native frame in the stack
    bool cleanFramesToLastNative();
};