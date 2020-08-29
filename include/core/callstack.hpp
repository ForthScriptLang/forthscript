#pragma once

#include <core/heap.hpp>
#include <core/values.hpp>

struct CallStack {
    Array** callStack = nullptr;
    size_t current = 0;

    CallStack(size_t recLimit);
    void pushFrame(Array* arr);
    void popFrame();
    void registerRootMarker(Heap& h);
};