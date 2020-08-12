#pragma once

#include <core/values.hpp>
#include <functional>
#include <vector>

#define HEAP_LOG(x) DBG_ONLY(std::cerr << "[ Heap ] " << x << std::endl)
#define GC_LOG(x) DBG_ONLY(std::cerr << "[  GC  ] " << x << std::endl)

struct Heap {
    Object *head;
    void insertObject(Object *obj);

    using RootMarker = std::function<void(struct Heap &)>;
    std::vector<RootMarker> rootMarkers;
    size_t insertRootMarker(RootMarker marker);

    void markObject(Object *obj);
    void collectGarbage();
    Heap();

    String *makeStringObject(const std::u32string &val);
    Array *makeArrayObject(Value defaultVal, size_t size);
    Array *shallowCopy(Array *other);
    Array *deepCopy(Array *other);
};