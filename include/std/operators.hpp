#include <core/interpreter.hpp>
#include <expect.hpp>
#include <functional>

#define MAKE_FROM_UNARY_OPERATOR(name, op)                         \
    ExecutionResult name(struct Interpreter& interp) {             \
        if_unlikely(!interp.evalStack.assertDepth(1)) {            \
            return ExecutionResult{ExecutionResultType::Error,     \
                                   U"Evaluation stack underflow"}; \
        }                                                          \
        Value first = interp.evalStack.popBack().value();          \
        Value result = op(first, interp);                          \
        interp.evalStack.pushBack(result);                         \
        return ExecutionResult{ExecutionResultType::Success,       \
                               U"Evaluation stack underflow"};     \
    }

#define MAKE_FROM_BINARY_OPERATOR(name, op)                        \
    ExecutionResult name(struct Interpreter& interp) {             \
        if_unlikely(!interp.evalStack.assertDepth(2)) {            \
            return ExecutionResult{ExecutionResultType::Error,     \
                                   U"Evaluation stack underflow"}; \
        }                                                          \
        Value second = interp.evalStack.popBack().value();         \
        Value first = interp.evalStack.popBack().value();          \
        Value result = op(first, second, interp);                  \
        interp.evalStack.pushBack(result);                         \
        return ExecutionResult{ExecutionResultType::Success,       \
                               U"Evaluation stack underflow"};     \
    }

#define MAKE_FROM_TERNARY_OPERATOR(name, op)                       \
    ExecutionResult name(struct Interpreter& interp) {             \
        if_unlikely(!interp.evalStack.assertDepth(3)) {            \
            return ExecutionResult{ExecutionResultType::Error,     \
                                   U"Evaluation stack underflow"}; \
        }                                                          \
        Value third = interp.evalStack.popBack().value();          \
        Value second = interp.evalStack.popBack().value();         \
        Value third = interp.evalStack.popBack().value();          \
        Value result = op(first, second, third, interp);           \
        interp.evalStack.pushBack(result);                         \
        return ExecutionResult{ExecutionResultType::Success,       \
                               U"Evaluation stack underflow"};     \
    }
