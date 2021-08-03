#include <prelude/repl.hpp>

std::u32string getPreludeREPLSource() {
    return U"[get_stack_copy to_string \"\n# \" +] $_repl_prompt [True] "
           U"[_repl_prompt! readln parse get_stack_size 1 - inline_try not "
           U"[\"error: \" swap + \". Clearing stack...\" + writeln] if] "
           U"inline_while";
}
