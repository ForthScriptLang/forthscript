#include <callstack.hpp>
#include <dbg.hpp>
#include <heap.hpp>
#include <iostream>
#include <symtable.hpp>
#include <values.hpp>

int main() {
    Heap heap;
    SymbolTable table;
    CallStack stack(1024);
    table.registerRootMarker(heap);
    stack.registerRootMarker(heap);

    Array* arr1 = makeArrayObject(Value(), 2);
    Array* arr2 = makeArrayObject(Value(), 1);
    String* str = makeStringObject(U"test string");

    Value arr1Pointer;
    arr1Pointer.arr = arr1;
    arr1Pointer.type = ValueType::Array;

    Value arr2Pointer;
    arr2Pointer.arr = arr2;
    arr2Pointer.type = ValueType::Array;

    Value strPointer;
    strPointer.str = str;
    strPointer.type = ValueType::String;

    Value none;
    none.type = ValueType::Nil;

    arr1->values[0] = arr2Pointer;
    arr1->values[1] = strPointer;

    arr2->values[0] = arr1Pointer;

    heap.insertObject(arr1);
    heap.insertObject(arr2);
    heap.insertObject(str);

    table.createScope();
    table.declareVariable(U"x", strPointer);
    table.declareVariable(U"y", arr1Pointer);

    heap.collectGarbage();

    DBG_ONLY(std::cerr << "==========================" << std::endl);
    stack.addArrayCallFrame(arr1, U"y");
    table.setVariable(U"y", none);
    heap.collectGarbage();

    return 0;
}
