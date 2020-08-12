#include <string.h>

#include <dbg.hpp>
#include <iostream>
#include <values.hpp>

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