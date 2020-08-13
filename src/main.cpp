#include <iostream>
#include <parser/lexer.hpp>

int main() {
    std::u32string toParse = U"[\"iu\"0+]dup 1\"rii\"set_at#hi hello\n!print";
    Lexems lexems = lex(toParse);
    for (const auto& lexem : lexems.lexems) {
        std::cout << (int)lexem.type << "\n";
    }
}