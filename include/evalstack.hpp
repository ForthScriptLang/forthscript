#pragma once

#include <deque>
#include <heap.hpp>
#include <values.hpp>

struct EvaluationStack {
    std::deque<Value> stack;

    void registerRootMarker(Heap &heap);
};