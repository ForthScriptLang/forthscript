#include <core/callstack.hpp>

CallStack::CallStack(size_t recLimit) {
    max = recLimit;
    callStack = new Array*[recLimit];
}

bool CallStack::pushFrame(Array* arr) {
    if (current == max) {
        return false;
    }
    callStack[current++] = arr;
    return true;
}

void CallStack::popFrame() { current--; }

void CallStack::registerRootMarker(Heap& h) {
    h.insertRootMarker([this](Heap& heap) {
        for (size_t i = 0; i < current; ++i) {
            heap.markObject(callStack[i]);
        }
    });
}