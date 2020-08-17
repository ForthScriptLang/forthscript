#include <core/symtable.hpp>

void SymbolTable::createScope() { numDefined.push_back(0); }

void SymbolTable::leaveScope() {
    for (size_t i = declared.size() - numDefined.back(); i < declared.size();
         ++i) {
        declared[i]->popValue();
    }
    numDefined.pop_back();
}

void SymbolTable::declareVariable(String* name, Value val) {
    if (name->values == nullptr ||
        name->values->back().second != numDefined.size()) {
        // variable was not declared in this scope yet
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