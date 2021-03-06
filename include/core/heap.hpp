#pragma once

#include <core/stringpool.hpp>
#include <core/values.hpp>
#include <functional>
#include <string>
#include <string_view>
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
    void runGCIfOverThreshold();
    Heap();

    StringPool pool;
    String *makeStringObject(const std::u32string &val);
    String *makeStringObject(std::u32string &&val);
    String *makeStringObject(const std::u32string_view &val);
    String *makeStringObject(const char32_t *val);

    size_t objectCount;
    size_t prevCount;

    Array *makeArrayObject(Value defaultVal, size_t size);
    Array *shallowCopy(Array *other);
    Array *deepCopy(Array *other);
};