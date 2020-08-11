#pragma once

#include <deque>
#include <heap.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <values.hpp>

class SymbolTable {
    std::deque<std::unordered_set<std::u32string>> declaredInScope;
    std::unordered_map<std::u32string, std::deque<Value>> values;

   public:
    void createScope();
    void declareVariable(const std::u32string& name, Value val);
    bool setVariable(const std::u32string& name, Value val);
    Value getVariable(const std::u32string& name);
    void registerRootMarker(Heap& heap);
};