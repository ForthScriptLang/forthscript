#pragma once

#include <core/heap.hpp>
#include <core/values.hpp>

struct CallStack {
    Array** callStack = nullptr;
    size_t current = 0;
	size_t max;

    CallStack(size_t recLimit);
    bool pushFrame(Array* arr);
    void popFrame();
    void registerRootMarker(Heap& h);
};