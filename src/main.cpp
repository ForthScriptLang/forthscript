#include <iostream>
#include <parser/parser.hpp>

int main() {
    Heap h;
    std::u32string toParse = U"[\"iu\"0+]dup 1\"rii\"set_at#hi hello\n!print";
    const std::u32string &asRef = toParse;
    ParseResult result = parse(asRef, h);
}