#pragma once

#include <deque>
#include <heap.hpp>
#include <values.hpp>

struct StackFrame {
    bool native;
    Array *code;
    std::u32string name;
};

struct CallStack {
    std::deque<StackFrame> frames;
    size_t recursionLimit;

    CallStack(size_t recLimit);
    void registerRootMarker(Heap &heap);
    bool addArrayCallFrame(Array *code, const std::u32string &name);
    bool addNativeCallFrame(const std::u32string &name);
    void removeTopCallFrame();
    void setRecursionLimit(size_t limit);
};