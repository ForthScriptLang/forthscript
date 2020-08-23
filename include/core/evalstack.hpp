#pragma once

#include <core/heap.hpp>
#include <core/values.hpp>
#include <optional>
#include <vector>

class EvaluationStack {
    std::vector<Value> stack;
    size_t barrier = 0;

   public:
    EvaluationStack();
    void registerRootMarker(Heap &heap);
    std::optional<Value> popBack();
    void pushBack(Value val);
    bool assertDepth(size_t count) const;
    void clear();
    size_t makeBarrier();
    void restoreBarrier(size_t size);
    size_t getStackSize();
    void resize(size_t new_size);

    inline const std::vector<Value> getStack() const { return stack; }
};