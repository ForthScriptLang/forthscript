#include <std/indexing.hpp>
#include <std/operators.hpp>

ExecutionResult getAtOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(2)) {
        return EvalStackUnderflow();
    }
    Value index = interp.evalStack.popBack().value();
    Value indexable = interp.evalStack.popBack().value();
    if (index.type != ValueType::Numeric) {
        return TypeError();
    }
    if (index.numericValue < 0) {
        return ExecutionResult{ExecutionResultType::Error, U"Out of bounds",
                               Value()};
    }
    switch (indexable.type) {
        case ValueType::Array:
            if (indexable.arr->values.size() <= (size_t)(index.numericValue)) {
                return ExecutionResult{ExecutionResultType::Error,
                                       U"Out of bounds", Value()};
            }
            interp.evalStack.pushBack(
                indexable.arr->values[index.numericValue]);
            return Success();
        case ValueType::String: {
            if (indexable.str->get().size() <= (size_t)(index.numericValue)) {
                return ExecutionResult{ExecutionResultType::Error,
                                       U"Out of bounds", Value()};
            }
            char32_t newCStr[2] = {indexable.str->get()[index.numericValue],
                                   '\0'};
            String* newStr =
                interp.heap.makeStringObject(std::u32string_view(newCStr));
            Value result;
            result.type = ValueType::String;
            result.str = newStr;
            interp.evalStack.pushBack(result);
            return Success();
        }
        default:
            break;
    }
    return ExecutionResult{ExecutionResultType::Error,
                           U"Value is not indexable", Value()};
}

ExecutionResult setAtOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(3)) {
        return EvalStackUnderflow();
    }
    Value element = interp.evalStack.popBack().value();
    Value index = interp.evalStack.popBack().value();
    Value indexable = interp.evalStack.popBack().value();
    if (index.type != ValueType::Numeric) {
        return TypeError();
    }
    if (index.numericValue < 0) {
        return ExecutionResult{ExecutionResultType::Error, U"Out of bounds",
                               Value()};
    }
    switch (indexable.type) {
        case ValueType::Array:
            if (indexable.arr->values.size() <= (size_t)(index.numericValue)) {
                return ExecutionResult{ExecutionResultType::Error,
                                       U"Out of bounds", Value()};
            }
            indexable.arr->values[index.numericValue] = element;
            return Success();
        default:
            return ExecutionResult{ExecutionResultType::Error,
                                   U"Value is not indexable or immutable",
                                   Value()};
    }
}

ExecutionResult lenOp(Interpreter& interp) {
    if (!interp.evalStack.assertDepth(1)) {
        return EvalStackUnderflow();
    }
    Value indexable = interp.evalStack.popBack().value();
    Value integerResult;
    integerResult.type = ValueType::Numeric;
    switch (indexable.type) {
        case ValueType::Array:
            integerResult.numericValue =
                (int64_t)(indexable.arr->values.size());
            interp.evalStack.pushBack(integerResult);
            return Success();
        case ValueType::String:
            integerResult.numericValue = (int64_t)(indexable.str->get().size());
            interp.evalStack.pushBack(integerResult);
            return Success();
        default:
            break;
    }
    return TypeError();
}

void addIndexingNativeWords(Interpreter& interp) {
    interp.defineNativeWord(U"len", lenOp);
    interp.defineNativeWord(U"peek", getAtOp);
    interp.defineNativeWord(U"poke", setAtOp);
}