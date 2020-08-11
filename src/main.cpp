#include <dbg.hpp>
#include <heap.hpp>
#include <iostream>
#include <values.hpp>

int main() {
    Heap heap;

    Array* arr1 = makeArrayObject(Value(), 2);
    Array* arr2 = makeArrayObject(Value(), 1);
    String* str = makeStringObject(U"test string");

    arr1->values[0].type = ValueType::Array;
    arr1->values[0].arr = arr2;

    arr1->values[1].type = ValueType::String;
    arr1->values[1].str = str;

    arr2->values[0].type = ValueType::Array;
    arr2->values[0].arr = arr1;

    heap.insertObject(arr1);
    heap.insertObject(arr2);
    heap.insertObject(str);

    heap.insertRootMarker([&](Heap& heap) { heap.markObject(arr1); });

    heap.collectGarbage();

    return 0;
}
