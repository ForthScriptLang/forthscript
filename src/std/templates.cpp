#include <expect.hpp>
#include <stack>
#include <std/templates.hpp>
#include <unordered_set>

struct FormatStatus {
    Object *formatted;
    ExecutionResult result;
};

FormatStatus formatArray(Array *format, Interpreter &interp) {
    std::unordered_set<Array *> encountered;
    struct FormatTask {
        Array *copy;
        Array *format;
        size_t pos;
    };
    std::stack<FormatTask> tasks;
    Array *copy = interp.heap.makeArrayObject(Value(), format->values.size());
    tasks.push(FormatTask{copy, format, 0});
    encountered.insert(format);
    while (!tasks.empty()) {
        FormatTask &task = tasks.top();
        if (task.pos >= task.format->values.size()) {
            tasks.pop();
            continue;
        }
        Value val = task.format->values[task.pos];
        switch (val.type) {
            case ValueType::Placeholder: {
                ExecutionResult result = interp.callInterpreter(val.arr, true);
                if_unlikely(result.result != ExecutionResultType::Success) {
                    return FormatStatus{nullptr, result};
                }
                if_unlikely(!interp.evalStack.assertDepth(1)) {
                    return FormatStatus{nullptr, EvalStackUnderflow()};
                }
                task.copy->values[task.pos] =
                    interp.evalStack.popBack().value();
            } break;
            case ValueType::Array: {
                if (encountered.find(val.arr) != encountered.end()) {
                    return FormatStatus{
                        nullptr,
                        ExecutionResult{ExecutionResultType::Error,
                                        U"Recursive template", Value()}};
                }
                encountered.insert(val.arr);
                Array *copy = interp.heap.makeArrayObject(
                    Value(), val.arr->values.size());
                tasks.push(FormatTask{copy, val.arr, 0});
                Value pointerToCopy;
                pointerToCopy.type = ValueType::Array;
                pointerToCopy.arr = copy;
                task.copy->values[task.pos] = pointerToCopy;
            } break;
            default:
                task.copy->values[task.pos] = val;
                break;
        }
        task.pos++;
    }
    return FormatStatus{copy, Success()};
}

ExecutionResult templateEvaluateOp(Interpreter &interp) {
    if (!interp.evalStack.assertDepth(1)) {
        return EvalStackUnderflow();
    }
    Value val = interp.evalStack.popBack().value();
    FormatStatus result;
    if (val.type != ValueType::Array) {
        return TypeError();
    }
    result = formatArray(val.arr, interp);
    if (result.result.result != ExecutionResultType::Success) {
        return result.result;
    }
    Value valResult;
    valResult.type = ValueType::Array;
    valResult.object = result.formatted;
    interp.evalStack.pushBack(valResult);
    return Success();
}

void addTemplatesNativeWords(Interpreter &interp) {
    interp.defineNativeWord(U"@", templateEvaluateOp);
}