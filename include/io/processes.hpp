#pragma once

#include <string>

struct ProcessInvokationRequest {
    std::u32string name;
};

struct ProcessInvokationResponce {
    std::u32string out;
    int errorCode;
    bool error;
};

ProcessInvokationResponce executeProcess(
    const ProcessInvokationRequest &command);