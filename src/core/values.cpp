#include <string.h>

#include <core/values.hpp>
#include <dbg.hpp>
#include <iostream>

Object::~Object() {}

String::~String() {}

Array::~Array() {}

Object *Array::addPointedToQueue(Object *start) {
    Object *new_tail = start;
    for (size_t i = 0; i < values.size(); ++i) {
        if (isHeapType(values[i].type) && !(values[i].object->marked)) {
            new_tail->next_to_scan = values[i].object;
            new_tail = values[i].object;
            new_tail->marked = true;
            new_tail->next_to_scan = nullptr;
        }
    }
    return new_tail;
}

Object *String::addPointedToQueue(Object *start) { return start; }

void String::pushValue(Value val, size_t scope) {
    if (values == nullptr) {
        values = new std::vector<std::pair<Value, size_t>>;
    }
    values->push_back(std::pair<Value, size_t>(val, scope));
}

Value &String::getLastValue() {
    if (values == nullptr) {
        throw std::runtime_error("no reference on the stack");
    }
    return std::get<0>(values->back());
}

void String::popValue() {
    values->pop_back();
    if (values->empty()) {
        delete values;
        values = nullptr;
    }
}