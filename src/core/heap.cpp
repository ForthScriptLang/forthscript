#include <algorithm>
#include <core/heap.hpp>
#include <dbg.hpp>
#include <iostream>
#include <queue>
#include <unordered_map>

Heap::Heap() { head = nullptr; }

void Heap::insertObject(Object *obj) {
    obj->next = head;
    head = obj;
}

size_t Heap::insertRootMarker(RootMarker marker) {
    rootMarkers.push_back(marker);
    return rootMarkers.size() - 1;
}

void Heap::markObject(Object *obj) {
    Object *queue_head = obj;
    Object *queue_tail = obj;
    queue_tail->next_to_scan = nullptr;
    obj->marked = true;
    while (queue_head != nullptr) {
        queue_tail = queue_head->addPointedToQueue(queue_tail);
        queue_head = queue_head->next_to_scan;
    }
}

void Heap::collectGarbage() {
    for (const RootMarker &marker : rootMarkers) {
        marker(*this);
    }
    Object *current = head;
    Object *prev = nullptr;
    while (current != nullptr) {
        if (current->marked) {
            current->marked = false;
            current->next_to_scan = nullptr;
            current = current->next;
        } else {
            Object *toDelete = current;
            current = current->next;
            if (prev == nullptr) {
                head = current;
            } else {
                prev->next = current;
            }
            delete toDelete;
        }
    }
}

String *Heap::makeStringObject(const std::u32string &val) {
    String *obj = new String(val);
    obj->marked = false;
    obj->next = obj->next_to_scan = nullptr;
    insertObject(obj);
    return obj;
}

String *Heap::makeStringObject(const std::u32string_view &val) {
    String *obj = new String(val);
    obj->marked = false;
    obj->next = obj->next_to_scan = nullptr;
    insertObject(obj);
    return obj;
}

Array *Heap::makeArrayObject(Value defaultVal, size_t size) {
    Array *arr = new Array;
    arr->marked = false;
    arr->next = arr->next_to_scan = nullptr;
    arr->values = std::vector<Value>(size, defaultVal);
    insertObject(arr);
    return arr;
}

Array *Heap::shallowCopy(Array *other) {
    Array *arr = new Array;
    arr->marked = false;
    arr->next = arr->next_to_scan = nullptr;
    arr->values.reserve(other->values.size());
    for (size_t i = 0; i < other->values.size(); ++i) {
        arr->values[i] = other->values[i];
    }
    insertObject(arr);
    return arr;
}

Array *Heap::deepCopy(Array *other) {
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