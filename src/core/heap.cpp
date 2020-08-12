#include <algorithm>
#include <dbg.hpp>
#include <heap.hpp>
#include <iostream>
#include <queue>
#include <unordered_map>

Heap::Heap() { head = nullptr; }

void Heap::insertObject(Object *obj) {
    HEAP_LOG("New object " << obj << " in the heap");
    obj->next = head;
    head = obj;
}

size_t Heap::insertRootMarker(RootMarker marker) {
    HEAP_LOG("New root marker added");
    rootMarkers.push_back(marker);
    return rootMarkers.size() - 1;
}

void Heap::markObject(Object *obj) {
    GC_LOG("Started recursive marking from object at " << obj);
    Object *queue_head = obj;
    Object *queue_tail = obj;
    queue_tail->next_to_scan = nullptr;
    obj->marked = true;
    while (queue_head != nullptr) {
        GC_LOG("Marking object at " << queue_head);
        queue_tail = queue_head->addPointedToQueue(queue_tail);
        queue_head = queue_head->next_to_scan;
    }
}

void Heap::collectGarbage() {
    GC_LOG("GC Started");
    GC_LOG("Mark Phase");
    for (const RootMarker &marker : rootMarkers) {
        GC_LOG("Calling Root Marker ");
        marker(*this);
    }
    GC_LOG("Sweep Phase");
    Object *current = head;
    Object *prev = nullptr;
    while (current != nullptr) {
        GC_LOG("Examining object at " << current);
        if (current->marked) {
            GC_LOG("Object at " << current << " survived");
            current->marked = false;
            current->next_to_scan = nullptr;
            current = current->next;
        } else {
            Object *toDelete = current;
            GC_LOG("Destroying object at " << toDelete);
            current = current->next;
            if (prev == nullptr) {
                head = current;
            } else {
                prev->next = current;
            }
            GC_LOG("Destroyed object at " << toDelete);
            delete toDelete;
        }
    }
    GC_LOG("GC Done");
}

String *Heap::makeStringObject(const std::u32string &val) {
    String *obj = new String;
    obj->str = val;
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