#include <algorithm>
#include <dbg.hpp>
#include <heap.hpp>
#include <iostream>

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