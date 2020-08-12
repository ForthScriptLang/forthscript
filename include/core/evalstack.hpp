#pragma once

#include <core/values.hpp>
#include <deque>
#include <heap.hpp>

struct EvaluationStack {
    std::deque<Value> stack;

    void registerRootMarker(Heap &heap);
};