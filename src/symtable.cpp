#include <symtable.hpp>

void SymbolTable::createScope() {
    declaredInScope.push_back(std::unordered_set<std::u32string>());
}

void SymbolTable::declareVariable(const std::u32string& name, Value val) {
    if (declaredInScope.back().find(name) == declaredInScope.back().end()) {
        declaredInScope.back().insert(name);
        values[name].push_back(val);
    } else {
        values[name].back() = val;
    }
}

bool SymbolTable::setVariable(const std::u32string& name, Value val) {
    if (values[name].empty()) {
        return false;
    }
    values[name].back() = val;
    return true;
}

Value SymbolTable::getVariable(const std::u32string& name) {
    if (values[name].empty()) {
        Value none;
        none.type = ValueType::Nil;
        return none;
    }
    return values[name].back();
}

void SymbolTable::registerRootMarker(Heap& heap) {
    heap.insertRootMarker([this](Heap& heap) {
        for (const auto& pair : values) {
            for (const auto& val : pair.second) {
                if (isHeapType(val.type) && !(val.object->marked)) {
                    heap.markObject(val.object);
                }
            }
        }
    });
}