#pragma once

#include <core/callstack.hpp>
#include <core/evalstack.hpp>
#include <core/heap.hpp>
#include <core/symtable.hpp>
#include <core/values.hpp>
#include <functional>
#include <string>
#include <unordered_map>

enum class ExecutionResultType { Error, Success, Return, Break };

struct ExecutionResult {
    ExecutionResultType result;
    std::u32string error;
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
    ExecutionResult callInterpreter(Array* code, const std::u32string& name,
                                    bool newScope);
};