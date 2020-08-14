#include <core/interpreter.hpp>
#include <functional>

using UnaryOperator = std::function<Value(Value, Interpreter&)>;
using BinaryOperator = std::function<Value(Value, Value, Interpreter&)>;
using TernaryOperator = std::function<Value(Value, Value, Value, Interpreter&)>;

NativeWord makeFromUnaryOperator(UnaryOperator op);
NativeWord makeFromBinaryOperator(BinaryOperator op);
NativeWord makeFromTernaryOperator(TernaryOperator op);