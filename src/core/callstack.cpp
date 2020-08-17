#include <core/callstack.hpp>

CallStack::CallStack(size_t recLimit) { recursionLimit = recLimit; }

void CallStack::registerRootMarker(Heap &heap) {
    heap.insertRootMarker([this](Heap &heap) {
        for (const auto &frame : frames) {
            if (!frame.native && !(frame.code->marked)) {
                heap.markObject(frame.code);
            }
        }
    });
}

bool CallStack::addArrayCallFrame(Array *code,
                                  bool newScope) {
    if (frames.size() == recursionLimit) {
        return false;
    }
    frames.push_back(StackFrame{false, newScope, 0, code});
    return true;
}

bool CallStack::addNativeCallFrame() {
    if (frames.size() == recursionLimit) {
        return false;
    }
    frames.push_back(StackFrame{true, false, 0, nullptr});
    return true;
}

bool CallStack::removeTopCallFrame() {
    bool newScope = frames.back().newScope;
    frames.pop_back();
    return newScope;
}

void CallStack::setRecursionLimit(size_t limit) { recursionLimit = limit; }