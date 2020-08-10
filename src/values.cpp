#include <string.h>

#include <dbg.hpp>
#include <iostream>
#include <values.hpp>

Object::~Object() {}

String::~String() { free((void *)str); }

Array::~Array() {}

String *makeStringObject(const char *str) {
    String *obj = new String;
    obj->length = strlen(str);
    obj->str = strdup(str);
    obj->marked = false;
    obj->next = obj->next_to_scan = nullptr;
    return obj;
}

Array *makeArrayObject(Value defaultVal, size_t size) {
    Array *arr = new Array;
    arr->marked = false;
    arr->next = arr->next_to_scan = nullptr;
    arr->values = std::vector<Value>(size, defaultVal);
    return arr;
}

void Array::iterateReferences(std::function<void(Object *&)> callback) {
    for (size_t i = 0; i < values.size(); ++i) {
        if (isHeapType(values[i].type)) {
            callback(values[i].object);
        }
    }
}

void String::iterateReferences(
    [[maybe_unused]] std::function<void(Object *&)> callback) {}