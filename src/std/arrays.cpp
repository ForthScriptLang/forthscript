#include <std/arrays.hpp>

ExecutionResult allocOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(2)) {
        return EvalStackUnderflow();
    }
    Value elem = interp.evalStack.popBack().value();
    Value size = interp.evalStack.popBack().value();
    if (size.type != ValueType::Numeric) {
        return TypeError();
    }
    if (size.numericValue < 0) {
        return ExecutionResult{ExecutionResultType::Error,
                               U"Array allocation with negative size"};
    }
    Array* result =
        interp.heap.makeArrayObject(elem, (size_t)(size.numericValue));
    Value pointerToArray;
    pointerToArray.arr = result;
    pointerToArray.type = ValueType::Array;
    interp.evalStack.pushBack(pointerToArray);
    return Success();
}

ExecutionResult copyOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(1)) {
        return EvalStackUnderflow();
    }
    Value val = interp.evalStack.popBack().value();
    if (val.type != ValueType::Array) {
        return TypeError();
    }
    Array* newArray = interp.heap.shallowCopy(val.arr);
    Value pointerToArray;
    pointerToArray.arr = newArray;
    pointerToArray.type = ValueType::Array;
    interp.evalStack.pushBack(pointerToArray);
    return Success();
}

ExecutionResult deepCopyOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(1)) {
        return EvalStackUnderflow();
    }
    Value val = interp.evalStack.popBack().value();
    if (val.type != ValueType::Array) {
        return TypeError();
    }
    Array* newArray = interp.heap.deepCopy(val.arr);
    Value pointerToArray;
    pointerToArray.arr = newArray;
    pointerToArray.type = ValueType::Array;
    interp.evalStack.pushBack(pointerToArray);
    return Success();
}

void addArrayManipulationNativeWords(Interpreter& interp) {
    interp.defineNativeWord(U"alloc", allocOp);
    interp.defineNativeWord(U"shallow_copy", copyOp);
    interp.defineNativeWord(U"deep_copy", deepCopyOp);
}