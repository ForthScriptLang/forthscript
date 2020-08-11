#include <string.h>

#include <dbg.hpp>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <values.hpp>

Object::~Object() {}

String::~String() {}

Array::~Array() {}

String *makeStringObject(const std::u32string &val) {
    String *obj = new String;
    obj->str = val;
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

Array *duplicateArray(Array *other) {
    Array *arr = new Array;
    arr->marked = false;
    arr->next = arr->next_to_scan = nullptr;
    arr->values.reserve(other->values.size());
    for (size_t i = 0; i < other->values.size(); ++i) {
        arr->values[i] = other->values[i];
    }
    return arr;
}

Array *recursiveCopy(Array *other) {
    DBG_ONLY(std::cerr << "[DeepCp] "
                       << "Copying array at " << other << std::endl);
    std::unordered_map<Array *, Array *> encountered;
    std::queue<std::pair<Array *, Array *>> copyTasks;
    Array *result = makeArrayObject(Value(), other->values.size());
    copyTasks.push(std::pair<Array *, Array *>(result, other));
    while (!copyTasks.empty()) {
        std::pair<Array *, Array *> copyTask = copyTasks.back();
        encountered[copyTask.second] = copyTask.first;
        copyTasks.pop();
        for (size_t i = 0; i < copyTask.second->values.size(); ++i) {
            copyTask.first->values[i] = copyTask.second->values[i];
            if (isArrayType(copyTask.first->values[i].type)) {
                Array *inner = copyTask.first->values[i].arr;
                if (encountered.find(inner) != encountered.end()) {
                    copyTask.first->values[i].arr = encountered[inner];
                } else {
                    std::pair<Array *, Array *> newCopyTask;
                    newCopyTask.first =
                        makeArrayObject(Value(), inner->values.size());
                    newCopyTask.second = inner;
                    copyTasks.push(newCopyTask);
                    copyTask.first->values[i].arr = newCopyTask.first;
                }
            }
        }
    }
    return result;
}

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