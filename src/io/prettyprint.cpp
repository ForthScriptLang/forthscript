#include <io/prettyprint.hpp>
#include <io/strings.hpp>
#include <iostream>
#include <stack>
#include <unordered_set>

void prettyprintNumeric(int64_t val, std::u32string& str,
                        bool firstCall = true) {
    if (val < 0) {
        str.push_back(U'-');
        val *= -1;
    }
    if (val == 0) {
        if (firstCall) {
            str.push_back(U'0');
        }
        return;
    }
    prettyprintNumeric(val / 10LL, str, false);
    str.push_back(U'0' + (char32_t)(val % 10));
}

void prettyprintPrimitive(const Value& val, std::u32string& str,
                          Interpreter& interp) {
    switch (val.type) {
        case ValueType::Boolean:
            if (val.booleanValue) {
                str.append(U"True");
            } else {
                str.append(U"False");
            }
            break;
        case ValueType::Numeric:
            prettyprintNumeric(val.numericValue, str);
            break;
        case ValueType::Nil:
            str.append(U"Nil");
            break;
        case ValueType::String: {
            std::u32string backslashed = convertToBackslashed(val.str->get());
            str.push_back(U'\"');
            str.append(backslashed);
            str.push_back(U'\"');
            break;
        }
        case ValueType::Word:
            str.append(val.str->get());
            break;
        case ValueType::WordAssign:
            str.push_back(U'=');
            str.append(val.str->get());
            break;
        case ValueType::WordDeclare:
            str.push_back(U'$');
            str.append(val.str->get());
            break;
        case ValueType::NativeWord: {
            str.append(interp.symbols[val.word]->get());
        } break;
        default:
            break;
    }
}

std::u32string prettyprint(Value val, Interpreter& interp) {
    std::u32string result;
    using PrintArrayTask = std::pair<Value&, size_t>;
    std::unordered_set<Array*> visited;
    std::stack<PrintArrayTask> tasks;
    if (!isArrayType(val.type)) {
        prettyprintPrimitive(val, result, interp);
        return result;
    }
    tasks.push(PrintArrayTask(val, 0));
    while (!tasks.empty()) {
        PrintArrayTask& topTask = tasks.top();
        visited.insert(topTask.first.arr);
        if (topTask.second == 0) {
            if (topTask.first.type == ValueType::Array) {
                result.append(U"[");
            } else {
                result.append(U"{");
            }
        }
        if (topTask.second >= topTask.first.arr->values.size()) {
            if (topTask.first.type == ValueType::Array) {
                result.push_back(U']');
            } else {
                result.push_back(U'}');
            }
            tasks.pop();
            if (!tasks.empty()) {
                if (tasks.top().second < tasks.top().first.arr->values.size()) {
                    result.push_back(U' ');
                }
            }
            continue;
        }
        Value& toPrint = topTask.first.arr->values[topTask.second];
        topTask.second++;
        if (!isArrayType(toPrint.type)) {
            prettyprintPrimitive(toPrint, result, interp);
            if (topTask.second < topTask.first.arr->values.size()) {
                result.push_back(U' ');
            }
        } else {
            if (visited.find(toPrint.arr) != visited.end()) {
                if (toPrint.type == ValueType::Placeholder) {
                    result.append(U"{...}");
                } else {
                    result.append(U"[...]");
                }
                if (topTask.second < topTask.first.arr->values.size()) {
                    result.push_back(U' ');
                }
            } else {
                tasks.push(PrintArrayTask(toPrint, 0));
            }
        }
    }
    return result;
}