#include <algorithm>
#include <core/heap.hpp>
#include <dbg.hpp>
#include <io/termio.hpp>
#include <iostream>
#include <queue>
#include <unordered_map>

Heap::Heap() {
    head = nullptr;
    objectCount = 0;
    // give some room before gc starts
    prevCount = 128;
}

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
    // we are handling symbol table stacks separately
    for (String *string : pool) {
        if (string->values != nullptr) {
            string->marked = true;
            for (auto val : *string->values) {
                if (isHeapType(val.first.type) && !val.first.object->marked) {
                    markObject(val.first.object);
                }
            }
        }
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
            --objectCount;
            delete toDelete;
        }
    }
    // Handling strings (they are referenced by pool instead of list)
    for (auto iter = pool.begin(); iter != pool.end();) {
        if ((*iter)->marked) {
            (*iter)->marked = false;
            ++iter;
        } else {
            auto toDelete = iter;
            ++iter;
            String *str = *toDelete;
            pool.erase(toDelete);
            --objectCount;
            delete str;
        }
    }
    prevCount = objectCount;
}

String *Heap::makeStringObject(const std::u32string &val) {
    return makeStringObject(std::u32string_view(val));
}

String *Heap::makeStringObject(const std::u32string_view &val) {
    String *obj = new String(val);
    obj->marked = false;
    obj->next = obj->next_to_scan = nullptr;
    auto result = pool.insert(obj);
    if (result.second) {
        objectCount++;
        return obj;
    } else {
        delete obj;
        return *result.first;
    }
}

String *Heap::makeStringObject(std::u32string &&val) {
    String *obj = new String(val);
    obj->marked = false;
    obj->next = obj->next_to_scan = nullptr;
    auto result = pool.insert(obj);
    if (result.second) {
        objectCount++;
        return obj;
    } else {
        delete obj;
        return *result.first;
    }
}

Array *Heap::makeArrayObject(Value defaultVal, size_t size) {
    Array *arr = new Array;
    objectCount++;
    arr->marked = false;
    arr->next = arr->next_to_scan = nullptr;
    arr->values = std::vector<Value>(size, defaultVal);
    insertObject(arr);
    return arr;
}

Array *Heap::shallowCopy(Array *other) {
    Array *arr = new Array;
    objectCount++;
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

void Heap::runGCIfOverThreshold() {
    if (objectCount > 2 * prevCount && objectCount > 256) {
        collectGarbage();
    }
}