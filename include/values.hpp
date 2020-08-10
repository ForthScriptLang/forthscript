#pragma once

#include <stdint.h>

#include <functional>
#include <string>
#include <vector>

struct Object {
    struct Object *next_to_scan, *next;
    bool marked : 1;

    virtual void iterateReferences(std::function<void(Object *&)> callback) = 0;
    virtual ~Object();
};

enum class ValueType {
    Nil = 1,
    Numeric = 2,
    Boolean = 3,
    Word = 128,
    WordAssign = 129,
    WordDeclare = 130,
    String = 131,
    Array = 132,
    Placeholder = 133
};

inline bool isHeapType(ValueType type) { return (((int)(type)) & 128) != 0; }

struct Value {
    ValueType type;
    union {
        bool booleanValue;
        int64_t numericValue;
        Object *object;
        struct String *str;
        struct Array *arr;
    };
    inline Value() { type = ValueType::Nil; }
};

struct String : Object {
    size_t length;
    const char *str;
    virtual ~String();
    virtual void iterateReferences(std::function<void(Object *&)> callback);
};

struct Array : Object {
    std::vector<Value> values;
    virtual ~Array();
    virtual void iterateReferences(std::function<void(Object *&)> callback);
};

String *makeStringObject(const char *str, size_t size);
Array *makeArrayObject(Value defaultVal, size_t size);