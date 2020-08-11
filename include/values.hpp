#pragma once

#include <stdint.h>

#include <functional>
#include <string>
#include <vector>

struct Object {
    struct Object *next_to_scan, *next, *prev;
    size_t referenceCount : 63;
    bool marked : 1;

    virtual Object *addPointedToQueue(struct Object *head) = 0;
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
    std::u32string str;
    virtual ~String();
    virtual Object *addPointedToQueue(struct Object *head);
};

struct Array : Object {
    std::vector<Value> values;
    virtual ~Array();
    virtual Object *addPointedToQueue(struct Object *head);
};

String *makeStringObject(const std::u32string &val);
Array *makeArrayObject(Value defaultVal, size_t size);