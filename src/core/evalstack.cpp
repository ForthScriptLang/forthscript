#include <core/evalstack.hpp>

EvaluationStack::EvaluationStack() { stack.reserve(1024); }

void EvaluationStack::registerRootMarker(Heap &heap) {
    heap.insertRootMarker([this](Heap &heap) {
        for (const auto &val : stack) {
            if (isHeapType(val.type) && !(val.object->marked)) {
                heap.markObject(val.object);
            }
        }
    });
}

std::optional<Value> EvaluationStack::popBack() {
    if (stack.size() <= barrier) {
        return std::optional<Value>();
    }
    Value result = stack.back();
    stack.pop_back();
    return result;
}

void EvaluationStack::pushBack(Value val) { stack.push_back(val); }

void EvaluationStack::clear() {
    stack.resize(barrier);
}

bool EvaluationStack::assertDepth(size_t count) const {
    return stack.size() - barrier >= count;
}

size_t EvaluationStack::getBarrier() { return barrier; }

void EvaluationStack::setBarrier(size_t size) { barrier = size; }

size_t EvaluationStack::getStackSize() { return stack.size(); }

void EvaluationStack::resize(size_t new_size) { stack.resize(new_size); }
