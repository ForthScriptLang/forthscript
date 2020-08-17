#include <algorithm>
#include <codecvt>
#include <io/termio.hpp>
#include <iostream>
#include <linenoise.hpp>
#include <locale>
#include <unordered_set>

bool noAutocomplete = false;

std::string fromUTF32(const std::u32string &s);
std::u32string toUTF32(const std::string &s);

std::u32string readLine(const std::u32string &prompt,
                        bool disableAutocomplete) {
    noAutocomplete = disableAutocomplete;
    std::string promptUtf8 = fromUTF32(prompt);
    std::string result;
    linenoise::Readline(promptUtf8.c_str(), result);
    linenoise::AddHistory(result.c_str());
    return toUTF32(result);
}

void print(const std::u32string &str) { std::cout << fromUTF32(str); }

void initREPL(Interpreter &interpreter) {
    linenoise::SetHistoryMaxLen(1024);
    linenoise::SetCompletionCallback(
        [&interpreter](const char *editBuffer,
                       std::vector<std::string> &completions) {
            if (noAutocomplete) {
                return;
            }
            size_t len = strlen(editBuffer);
            size_t lastSuccessfulStart = len;
            for (size_t ind = len; ind > 0; --ind) {
                size_t i = ind - 1;
                if (isalpha(editBuffer[i]) || editBuffer[i] == '_') {
                    lastSuccessfulStart = i;
                } else if ('0' > editBuffer[i] || editBuffer[i] < '9') {
                    break;
                }
            }
            if (lastSuccessfulStart == len) {
                return;
            }
            // slice from i to string end is a valid ident start
            std::string slice(editBuffer + lastSuccessfulStart);
            std::u32string prefix = toUTF32(slice);
            std::unordered_set<String *> visited;
            std::string beforeCompleition =
                std::string(editBuffer).substr(0, lastSuccessfulStart);
            for (String *str : interpreter.symTable.getDeclared()) {
                if (visited.find(str) != visited.end()) {
                    continue;
                }
                visited.insert(str);
                if (str->get().substr(0, prefix.size()) == prefix) {
                    std::string completition = fromUTF32(str->get());
                    completions.push_back(beforeCompleition + completition);
                }
            }
            std::sort(completions.begin(), completions.end());
        });
}