#include <api/dll.hpp>
#include <io/encodings.hpp>

#ifndef __LINUX__
#include <dlfcn.h>
#else
#error Other platforms are not currently supported
#endif

DLL *makeDLL(const std::u32string &name) {
    std::string u8name = fromUTF32(name);
    DLL *val = new DLL;
#ifndef __LINUX__
    val->handle = dlmopen(LM_ID_NEWLM, u8name.c_str(), RTLD_LOCAL);
#else
#error Other platforms are not currently supported
#endif
    return val;
}

DLL::~DLL() {
#ifndef __LINUX__
    dlclose(handle);
#else
#error Other platforms are not currently supported
#endif
}

void *DLL::getFunction(const std::u32string &name) {
    std::string u8name = fromUTF32(name);
#ifndef __LINUX__
    return dlsym(handle, u8name.c_str());
#else
#error Other platforms are not currently supported
#endif
}