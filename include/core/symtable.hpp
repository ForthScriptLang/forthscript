#pragma once

#include <core/heap.hpp>
#include <core/values.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class SymbolTable {
    std::vector<String *> declared;
    std::vector<size_t> numDefined;

   public:
    SymbolTable(size_t recLimit) { numDefined.reserve(recLimit); }
    void createScope();
    void leaveScope();
    void declareVariable(String *name, Value val);
    void setVariable(String *name, Value val);
    Value getVariable(String *name);
    inline const std::vector<String *> &getDeclared() { return declared; };
};