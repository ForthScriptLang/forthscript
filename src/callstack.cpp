#include <callstack.hpp>

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

bool CallStack::addArrayCallFrame(Array *code, const std::u32string &name) {
    if (frames.size() == recursionLimit) {
        return false;
    }
    frames.push_back(StackFrame{false, code, name});
    return true;
}

bool CallStack::addNativeCallFrame(const std::u32string &name) {
    if (frames.size() == recursionLimit) {
        return false;
    }
    frames.push_back(StackFrame{true, nullptr, name});
    return true;
}

void CallStack::removeTopCallFrame() { frames.pop_back(); }

void CallStack::setRecursionLimit(size_t limit) { recursionLimit = limit; }