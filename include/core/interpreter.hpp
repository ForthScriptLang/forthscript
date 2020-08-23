#pragma once

#include <core/evalstack.hpp>
#include <core/heap.hpp>
#include <core/symtable.hpp>
#include <core/values.hpp>
#include <functional>
#include <string>
#include <unordered_map>

enum class ExecutionResultType {
    Error,
    Success,
    Return,
    Break,
    Continue,
    Custom
};

struct ExecutionResult {
    ExecutionResultType result = ExecutionResultType::Success;
    const char32_t* error = U"";
    Value val;
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
    EvaluationStack evalStack;
    Heap heap;
    SymbolTable symTable;
    std::unordered_map<NativeWord, String*> symbolsToStrings;
    std::unordered_map<String*, NativeWord> stringsToSymbols;
    size_t recursionDepth;
    size_t maxRecursionDepth;

    Interpreter(size_t maxRecursionDepth);
    void defineNativeWord(const std::u32string& str, NativeWord word);
    ExecutionResult callInterpreter(Array* code, bool newScope);
    NativeWord queryNativeWord(String* str);
};