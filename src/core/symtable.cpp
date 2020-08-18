#include <core/symtable.hpp>
#include <dbg.hpp>
#include <io/termio.hpp>
#include <iostream>

void SymbolTable::createScope() { numDefined.push_back(0); }

void SymbolTable::leaveScope() {
    for (size_t i = declared.size() - numDefined.back(); i < declared.size();
         ++i) {
        declared[i]->popValue();
    }
    declared.resize(declared.size() - numDefined.back());
    numDefined.pop_back();
}

void SymbolTable::declareVariable(String* name, Value val) {
    if (name->values == nullptr ||
        name->values->back().second != numDefined.size()) {
        // variable was not declared in this scope yet
        numDefined[numDefined.size() - 1]++;
        declared.push_back(name);
        name->pushValue(val, numDefined.size());
    } else {
        name->getLastValue() = val;
    }
}

void SymbolTable::setVariable(String* name, Value val) {
    if (name->values == nullptr) {
        name->pushValue(val, numDefined.size());
    } else {
        name->getLastValue() = val;
    }
}

Value SymbolTable::getVariable(String* name) {
    if (name->values == nullptr) {
        return Value();
    } else {
        return name->getLastValue();
    }
}