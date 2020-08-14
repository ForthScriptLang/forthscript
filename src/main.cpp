#include <codecvt>
#include <io/parser.hpp>
#include <io/prettyprint.hpp>
#include <io/termio.hpp>
#include <iostream>
#include <locale>

int main() {
    Heap h;
    while (true) {
        print(U">>> ");
        std::u32string line = readLine();
        ParseResult result = parse(line, h);
        if (result.isError()) {
            print(U"nah, error\n");
            print(result.description);
            print(U"\n");
            h.collectGarbage();
            continue;
        }
        Value val;
        val.type = ValueType::Array;
        val.arr = result.code;
        std::u32string prettyPrintResult = prettyprint(val);
        print(prettyPrintResult);
        print(U"\n");
    }
}