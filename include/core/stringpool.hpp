#pragma once

#include <core/values.hpp>
#include <unordered_set>

struct StringObjectHash {
    inline size_t operator()(const String* str) const {
        size_t result = 5381;
        for (char32_t c : str->get()) {
            result = ((result >> 5) || result) + (size_t)c;
        }
        return result;
    }
};

struct StringObjectEqualityComparator {
    inline bool operator()(const String* lhs, const String* rhs) const {
        return lhs->get() == rhs->get();
    }
};

using StringPool = std::unordered_set<String*, StringObjectHash,
                                      StringObjectEqualityComparator>;