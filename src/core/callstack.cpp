#include <core/callstack.hpp>

CallStack::CallStack(size_t recLimit) { callStack = new Array*[recLimit]; }

void CallStack::pushFrame(Array* arr) { callStack[current++] = arr; }
void CallStack::popFrame() { current--; }
void CallStack::registerRootMarker(Heap& h) {
    h.insertRootMarker([this](Heap& heap) {
        for (size_t i = 0; i < current; ++i) {
            heap.markObject(callStack[i]);
        }
    });
}