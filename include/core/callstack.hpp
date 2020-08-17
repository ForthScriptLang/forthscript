#pragma once

#include <core/heap.hpp>
#include <core/values.hpp>
#include <deque>

struct StackFrame {
    bool native;
    bool newScope;
    size_t ip;
    Array *code;
};

struct CallStack {
    std::deque<StackFrame> frames;
    size_t recursionLimit;

    CallStack(size_t recLimit);
    void registerRootMarker(Heap &heap);
    bool addArrayCallFrame(Array *code, bool newScope);
    bool addNativeCallFrame();
    bool removeTopCallFrame();
    void setRecursionLimit(size_t limit);
};