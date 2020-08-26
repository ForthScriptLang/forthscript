#pragma once

#include <string>

class DLL {
    void *handle;

    void *getFunction(const std::u32string &name);
    ~DLL();

    friend DLL *makeDLL(const std::u32string &name);
};