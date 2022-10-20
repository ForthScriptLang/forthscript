# forthscript
[![Run on Repl.it](https://repl.it/badge/github/ForthScriptLang/forthscript)](https://repl.it/@ForthScriptLang/forthscript) ![C/C++ CI](https://github.com/ForthScriptLang/forthscript/workflows/C/C++%20CI/badge.svg?branch=master)

This repository contains code for the reference implementation of the ForthScript programming language).

ForthScript is a stack-based programming language with automatic memory management, heavily inspired by Forth and Lisp. ForthScript and Forth are both stack-based concatenative programming languages. ForthScript extends Forth with additional features that make ForthScript slightly higher level - local and global variables, array operations and tracing garbage collection.

ForthScript borrows homoiconicity from Lisp - any ForthScript function is an array of instructions. In fact, there is no difference between code and data - all values in ForthScript can be interpreted as either.

The ForthScript is dynamically typed (variables and arrays can store values of any kind). Yet, types are still a thing in ForthScript, and so are type errors - one cannot add integer to a string. 

### Building ForthScript

ForthScript interpreter can be built as any other CMake C/C++ project. All you need is CMake and a C++ compiler. No third-party dependencies required.

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Build directory should now contain `forthscript` (or `forthscript.exe`) executable.

## Running ForthScript

To launch ForthScript interpreter in REPL (Read-Eval-Print-Loop) mode, simply run the `forthscript` executable with no arguments 

```bash
# Run REPL
./forthscript
```

To execute a file with ForthScript code, pass the path to the file as the first argument
```bash
# Run example.fscript
./forthscript example.fscript
```

```.fscript``` is a standard file extension for ForthScript source files.

## More on ForthScript REPL

You can try all examples below with REPL. When you run the interpreter in REPL mode (no CLI arguments), you should see
the REPL prompt

```
[]
#
```

The first line visualises values on the stack. As the interpreter stack is empty on startup, there won't be anything between the brackets.

The second line is the prompt. Everything after `# ` is interpreted as ForthScript code.

To quit the REPL session, type `quit` and press Enter.

## Examples

### "Hello, world"
```
"Hello, world!" writeln
```

### Swap function
```
# $a declares a local variable and sets it to TOS (which gets removed)
[$a $b a b] $swap
"3" "4" swap! writeln writeln
```

### FizzBuzz
```
# =a assings TOS to a
# for loop creates two scopes - one with $i and one with loop body
[1 $i] [i 30 <] [i 1 + =i] [
	i 15 % 0 == ["FizzBuzz" writeln continue] if
	i 5 % 0 == ["Buzz" writeln continue] if
	i 3 % 0 == ["Buzz" writeln continue] if
	i to_string writeln
] for
```

### Infix arithmetic expressions (using homoiconicity)
```
# Run examples/expressions_compiler/main.fscript in this scope
# "parse" instruction parses string and outputs ForthScript array with instructions
# "," runs this array in this scope
"examples/expressions_compiler/main.fscript" readfile parse ,

# arith_compile function is now available in this scope
"arith_compile source:" writeln
arith_compile serialize writeln

# use arith_compile to transform arithmetic expressions
"compiling: " write
[2 + 2 * 2] $source source serialize writeln
"got: " write
source arith_compile! $compiled compiled serialize writeln
"result: " write
# arith_compile also returns an array of instructions, which we can just run
compiled! to_string writeln # 6
```
