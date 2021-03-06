#include <cstdlib>
#include <io/processes.hpp>

std::u32string toUTF32(const std::string &s);
std::string fromUTF32(const std::u32string &s);

#ifdef _WIN32
#define POPEN _popen
#define PCLOSE _pclose
#else
#define POPEN popen
#define PCLOSE pclose
#endif

ProcessInvokationResponce executeProcess(
    const ProcessInvokationRequest &command) {
    ProcessInvokationResponce result;

    FILE *file = POPEN(fromUTF32(command.name).c_str(), "r");
    if (file == NULL) {
        result.error = true;
        return result;
    }
    char buffer[128];
    std::string out;
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        out += buffer;
    }
    result.errorCode = PCLOSE(file);
    result.out = toUTF32(out);
    result.error = false;
    return result;
}