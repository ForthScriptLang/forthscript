#pragma once

#include <callstack.hpp>
#include <evalstack.hpp>
#include <functional>
#include <heap.hpp>
#include <string>
#include <symtable.hpp>
#include <unordered_map>
#include <values.hpp>

enum class ExecutionResultType { Error, Success };
enum class ExecutionErrorType {
    Success,
    DivisionByZero,
    CallStackOverflow,
    TypeError,
    EvalStackUnderflow,

};

struct ExecutionResult {
    ExecutionResultType result;
    ExecutionErrorType error;
};

using NativeWord = std::function<ExecutionResult(struct Interpreter&)>;

struct Interpreter {
    CallStack callStack;
    EvaluationStack evalStack;
    Heap heap;
    SymbolTable symTable;
    std::unordered_map<std::u32string, NativeWord> nativeWords;

    Interpreter(size_t maxRecursionDepth);
    void defineNativeWord(const std::u32string& name, NativeWord word);
    ExecutionResult callInterpreter(Array* code, bool newScope = true);
};