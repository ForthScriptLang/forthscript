#include <evalstack.hpp>

void EvaluationStack::registerRootMarker(Heap &heap) {
    heap.insertRootMarker([this](Heap &heap) {
        for (const auto &val : stack) {
            if (isHeapType(val.type) && !(val.object->marked)) {
                heap.markObject(val.object);
            }
        }
    });
}