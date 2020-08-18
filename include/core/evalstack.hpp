#pragma once

#include <core/heap.hpp>
#include <core/values.hpp>
#include <optional>
#include <vector>

class EvaluationStack {
    std::vector<Value> stack;

   public:
    EvaluationStack();
    void registerRootMarker(Heap &heap);
    std::optional<Value> popBack();
    void pushBack(Value val);
    bool assertDepth(size_t count) const;
    void clear();

    inline const std::vector<Value> getStack() const { return stack; }
};