# forthscript
[![Run on Repl.it](https://repl.it/badge/github/ForthScriptLang/forthscript)](https://repl.it/@ForthScriptLang/forthscript) ![C/C++ CI](https://github.com/ForthScriptLang/forthscript/workflows/C/C++%20CI/badge.svg?branch=master)

This is a repository with code for ForthScript language interpreter (previously known as overstacked).

ForthScript is a stack-based programming language with automatic memory management, heavily inspired by forth and lisp. 

The main paradigm of the language is code as data. Every program in ForthScript can be represented in ForthScript types, and every value in ForthScript can be interpreted as instructions to ForthScript interpreter.

The ForthScript is dynamically typed, but because of strict typing, operations like adding strings and integers are prohibited
### How to use the interpreter?
Running interpreter with no command line arguments will open REPL with line editing. Otherwise, interpreter will take the first argument as a path to file that should be executed
```
# Run repl
forthscript
# Run file.fscript
forthscript file.fscript
``` 
```.fscript``` is a standard file extension for forthscript source files.

All examples here are run in REPL. It can be exited by typing ```quit```.
```
[]# quit
```
Part ```[]``` is used to visualize interpreter stack, and everything after ```# ``` is interpreted as user input.

In code itself, everything after ```#``` to the next newline is interpreted as a comment and ignored.
## Language tour
### Basic Math
Number type is equivalent to int64_t in C
When interpreted as instruction, it will push itself on evaluation stack
```
[]# 3
[3]# 2
[3 2]#
```
Of course, more than one instruction to interpreter can be written at a time.
```
[]# 3 2
[3 2]#
```
To do basic calculations, we need to introduce another type: NativeWord. NativeWord is internally a pointer to some native function. When value of this type is interpreted as instruction, this native function is called.

For instance, ```+``` is a literal of type NativeWord, that will point to function, performing addition of two elements on the top of the stack. ```quit``` is also a native word, that just exits the interpreter.

With this knowledge, we can finally add two numbers.
```
[]# 2 3
[2 3]# + # When '+' is interpreted, it will add up 2 and 3
[5]#
```
Here are all arithmetic operations in forthscript.
##### Addition
```
[]# 2 3 +
[5]# 
```
##### Substraction
```
[]# 8 3 -
[5]#
```
##### Multiplication
```
[]# 7 4 *
[28]#
```
##### Division
```
[]# 15 4 /
[]# 3
```
##### Modulo operator
```
[]# 12 7 %
[]# 5
```
More complex expressions can be calculated by chaining these operations. This calculates (8 - 2) * (5 / (3 + 1)), leaving the result on the stack
```
[]# 8 2 - 5 3 1 + / *
[6]#
```
### Booleans
Boolean is another ForthScript type with only two values: ```True``` and ```False```. Like number, the value of this type is pushed on the stack when interpreted as instruction.
```
[]# True
[True]# False
[True False]#
```
Boolean is a return type for comparisons
```
[]# 3 4 !=
[True]# 3 4 ==
[True False]# 10 5 <
[True False False]# 10 5 >
[True False False True]# 10 11 >=
[True False False True False]# 10 11 <=
[True False False True False True]# 
```
The stack can get pretty messy after these operations. Luckily, there is a native word ```clear``` which was created specifically for solving this problem.
```
[True False False True False True]# clear
[]#
```
Logical operations such as ```not```, ```and``` and ```or``` are defined
```
[]# True False and
[False]# clear
[]# True False or
[True]# not
[False]#
```
### Strings
Like in python, Java, C#, etc the strings in ForthScript are immutable. All values of String Type just point to string pool, where all strings are interned. Like numbers and booleans, when interpreted as an instruction, the reference to the stirngs will be pushed on the evaluation stack.

Unlike many other programming languages, strings in ForthScript are represented as 4 byte char arrays (UTF-32). There may be some issues with encodings, especially on Windows, where UTF-8 is not a default encoding, but it should be fixed with good encoding library soon. Stay tuned.
```
[]# "Hello, world"
["Hello, world"]#
```
Strings can be concatenated using ```+```native word, and multiplied using ```*``` native word. 
```
[]# "I will do backups, " "I swear!\n" +
["I will do backups, I swear!\n"]# 2 *
["I will do backups, I swear!\nI will do backups, I swear!\n"]#
```
Length of the string can be obtained using ```len``` native word. To get nth character in the string, word ```peek``` can be used (the return type is also string, as there is no character type in forthscript). ```peek``` takes index (which is at the top of the stack) and reference to string (which is right below the top). 
```
[]# "abacaba" len
[7]# "what is a string" 2 peek
[7 "a"]# 
```
Native word ```slice``` can be used to slice a string 
```
[]# "Hello, world!" 0 4 slice
["Hell"]#
```
Unlike many other programming languages, strings in ForthScript are represented as 4 byte char arrays (UTF-32). There may be some issues with encodings, especially on Windows, where UTF-8 is not a default encoding, but it should be fixed with good encoding library soon. Stay tuned.
```
[]# "λ" len
[1]#
```
Words ```ord``` and ```chr``` can be used to convert characters to integers and back
```
[]# "a" ord
[97]# chr
["a"]#
```
There are also two different operations on strings, called ```split``` and ```join```, but to use them, we need another type...
### Arrays
Array is a type equivalent to ```list``` in python or ```std::vector<any>``` in c++. It is a resizable indexable collection, that may conatin elements of different types.

Arrays literals is a list of any forthscript values separated by spaces and enclosed by ```[]```. For instance, ```[2 3 [1 2 3 "Hello, there"] ["How was your day?" True not] False +]``` is indeed a valid array literal with numbers, strings, booleans, arrays and native words in it. 

Expression ```[1 2 3]``` won't instantiate a new array every time it is encountered. Instead, it is just referenced by code (which is also in fact forthscript array). Like numbers, strings and booleans, when interpreter as instruction, it will push reference to this array on the stack.
```
[]# [1 2 3]
[[1 2 3]]#
```
```len```, ```peek```, ```+```, ```slice```, and ```*``` words can be also used on arrays.
```
[]# ["string"] [1 2 3] len * ["oof"] +
[["string" "string" "string" "oof"]]# 3 peek
["oof"]#
```
```
[]# [1 2 3 4 5] 1 4 slice
[[2 3 4]]#
```
The word ```poke``` allows to set element of an array.
As it has three arguments, it is important to remember their order. From bottom to top there is reference to the array, index, and new element. 

To run this example, another word ```dup``` is needed. It just duplicates the top element on the stack.
```
[]# [1 2 3] dup # Now we have two references to the same array
[[1 2 3] [1 2 3]]# 0 "New element" poke
[["New element" 2 3]]#
```
```poke``` erases all arguments from the stack, pushing nothing back. Thus, to make sure, that changes to array are observable, the reference to array in this example is duplicated on the stack. One reference goes as an argument to poke, and another is left on the stack. This reference points to original array with ```1``` replaced by ```"New elemenent"``` using ```poke``` native word.

```deep_copy``` and ```shallow_copy``` can be used to copy the array. The difference is that ```shallow_copy``` will only copy elements of the array passed as an argument, and ```deep_copy``` will copy the whole object hierarchy

```append``` appends one element to the original array.
```resize``` resizes the array, given size and new element (used to fill the gap).
```alloc``` can be used to create an array with N given elements
```
[]# [1 2 3] dup 4 append
[[1 2 3 4]]# dup 6 "Filling the gap" resize
[[1 2 3 4 "Filling the gap" "Filling the gap"]]# 
```
```
[]# 3 10 alloc
[[10 10 10]]# 
```
```split``` can be used to split a given string using a string of delimeters.
```join``` can be used to add concatenate list of strings with a given joint string
```
[]# "ab,a,ca ba" ", " split
[["ab" "a" "ca" "ba"]]# "," join
["ab,a,ca,ba"]# 
```
```@``` operator can be used to initialize arrays from templates. ```Placeholder``` is a type used in arrays to indicate where new elements should be inserted. Placeholders can contain code that will be evaluated when elements array is constructed. When executed, placeholder just pushes reference to itself on the stack.
```
[]# 1 2 3 [{} [{} {}]] @
[[3 [2 1]]]# clear
[]# 1 [{} [{2} {3 4 +}]] @
[[1 [2 7]]]#
```
```SplicePlaceholder``` type can be used to include array contents in templates directly when calling template evaluation operator.
```
[]# [2 3] $args
[]# [(args) +]@
[[2 3 +]]# !
[5]
```
```Placeholder``` and ```SplicePlaceholder``` literals are similar to ```Array``` literals. The only difference is that they use different braces: ```{}``` in the case of placeholders and ```()``` in the case of splice placeholders.
### Variables

In all examples above, we were just writing expressions, that operated on the stack. Only the top of the stack was accessible. This is about to change with introduction of variables and two types: Word and WordAssign.

Word is a type, that stores a string internally. When it is executed, however, Word is very different from String: interpreter will lookup the variable with a name stored in the string.

WordAssign also stores a string, representing variable name. When it is executed, interpreter will pop the top value from the stack and store it inside the variable with a name stored inside the string

Word literal can be written by writing any valid identifier name (that is combination of characters, digits and _, where the first character is not a digit). WordAssign literal can be written by writing ```=``` before the identifier.

```
[]# 3 =a # =a is a value of WordAssign type
[]# a a * # a is a value of Word type
[9]#
```
### Functions
There is no notion of function in forthscript language, however there is ```!``` operator to execute an array of code

```
[]# [100 +] =add_100
[]# 200 add_100
[200 [100 +]]# !
[300]#
```
There is no magic in this example: the first line sets variable add_100 to the reference to the array ```[100 +]```. There are two elements: one of type Numeric (```100```), and another of type NativeWord (```+```). The next line pushes 200 on the stack, and then pushes reference to the array with code on the stack. ```!``` on the next line takes this array from the top of the stack and executes code inside it. ```100``` pushes 100 on the stack when executed (see Basic Math section) and ```+``` calls native function to perform addition on two top stack elements, which in this case are 100 and 200. The result, 300, is pushed back to the stack.
### Local variables
NativeWord ```!``` also creates a new scope when calls the function. To use local variables, WordDeclare type is introduced.

WordDeclare type is similar to WordAssign, however, unlike WordAssign, that will set a variable in the scope it was last declared (and declare in this scope if it wasn't declared at all), WordDeclare will declare a new variable with the name stored internally. On exit from ```!``` call, scope is removed, so variables declared with WordAssign are effectively gone. If variable was declared in this scope already, it acts as a simple assignment.

WordAssign literal can be written as ```$id```, where id is any valid identifier.

For instance, here is an example where the attempt is done to write a function to square a number.

```
[]# [=a a a *] =sqr
[]# 3 sqr!
[9]#
```
This function works perfectly in this case, but if variable a is used in the code that calls sqr, it may return incorrect results
```
[]# 5 =a a
[5]# clear [=a a a *] =sqr 3 sqr!
[9]# a
[9 3]#
```
Previously the value of a was 5, but now it is equal to 3, as it was modified in sqr routine. To solve the issue, sqr can be rewritten as ```[$a a a *]```
```
[]# 5 =a a
[5]# clear [$a a a *] =sqr 3 sqr!
[9]# a
[9 5]#
```
Here, a is redeclared inside sqr call scope, so on return the old value of a is returned.

Unlike many other programming languages, where child function can access variables that could be accessed in a scope where function was actually defined, in forthscript callee can access all variables from caller, meaning that this code is perfectly valid
```
[]# [a a *] $func2 # accesses variable 'a' defined in func1
[]# [$a func2!] $func1
[]# 2 func1!
[4]# 
```
Where there is no need to create a new scope, native word ```,``` can be used. It is equivalent to ```!```, but won't create a new scope
```
[]# 5 $a
[]# [4 $a], a
[4]#
```
### Stack manipulation
There are some operators designed to move stack elements.
```
[2]# dup # (a -- a a)
[2 2]# drop # (a -- )
[2]# 3
[2 3]# over # (a b -- a b a)
[2 3 2]# swap # (a b -- b a)
[2 2 3]#
```
### Nil type
Type ```Nil``` is equivalent to ```None``` in python. Like for many other simple types, it is just pushed on the stack when executed
```
[]# Nil
[Nil]# 
```
Nil can be also obtained by retrieving a variable that was not defined
```
[]# some_undefined_variable
[Nil]#
```
### Control flow

To be turing-complete, ForthScript needs some control flow constructs. They are implemented as native words, that take code and conditions from the top of the stack and implement logic of corresponding control flow operator. For example, this is how if looks like
```
[]# True [42] if
[42]# drop False [42] if
[]#
```
```if``` takes condition and array with code from the stack. If condition is true, it executes the array with code, otherwise it just terminates.
```if_else``` can be used for if else like constructs
```
[]# True [42] [19] if_else
[42]# False [42] [19] if_else
[42 19]#
```
This is enough for turing-complete language, but there are also loop native words in ForthScript.
```while``` takes clause used to calculate condition and body of the loop itself (both represented as array of code) from the stack.

```
[]# 0 $i
[]# [i 10 <] [i dup * i 1 + =i] while
[0 1 4 9 16 25 36 49 64 81]# 
```
For ```if```, ```if_else``` and ```while```, every action runs inside a new scope. 

The code above can be also rewritten with for loop, that takes code for initial action, condition calculation and code for each iteration, thus representing C style for loop. Condition, iteration and initial actions run in the same scope, but loop body has a new separate scope.
```
[]# [0 $i] [i 10 <] [i 1 + =i] [i dup *] for
[0 1 4 9 16 25 36 49 64 81]# 
```
```break``` if used to exit while/for loop and ```continue``` is used to move to the next iteration.
```
[]# [0 $i] [i 10 <] [i 1 + =i] [i continue dup *] for
[0 1 2 3 4 5 6 7 8 9]# 
```
```return``` is used to return from ```!``` or ```,``` function call.

```
[]# [3 return 4]! # 4 in this case is unreachable
[3]# 
```
### Type introspection
```type``` native word can be used to retrieve the string, representing type of the top element on the stack (this element is removed from the stack)

```
[]# 4 type
["Numeric"]# clear [type "Numeric" ==] $is_numeric
[]# 3 is_numeric!
[True]# clear "string" is_numeric!
[False]# 
```
### IO and friends
```write``` can be used to print string to standard output.
```
[]# "Hello, world\n" write
Hello, world
[]# 
```
```writeln``` is equivalent to ```write```, but also outputs ```\n``` in the end.
```
[]# "Hello, world" writeln
Hello, world
[]# 
```
```readln``` can be used to get a string from standard input
```
[]# readln
I am writing something
["I am writing something"]# 
```
```readfile``` can be used to read a file in current working directory
```
[]# "file.txt" readfile
["<contents of file.txt escaped>"]# 
```
```writefile``` can be used to write string to a file
```
[]# "hey" "test.txt" writefile
[<status of writing "hey" to "test.txt" (True/False)>]# 
```
```quit``` can be used to exit interpreter with return code 0, and ```exit``` can be used to exit interpreter with user defined code
```
[]# quit
exited with code 0
```
```
[]# -1 exit
exited with code -1
```
```exec``` can be used to run a process. The first argument (TOS) is command with arguments as a string, the second is a string that would be written to stdin.
On success, it pushes stdout&err contents, return code, and boolean ```True```. On failure, it pushes only one boolean ```False``` 
```
[]# "" "./build/forthscript tests/cases/brainfuck/input.txt" exec
["hello world" 0 True]# 
```
```glob``` can be used to glob files. TOS is a pattern, and list with filename strings are pushed on the stack
```
[[]]# "tests/cases/*" glob
[["brainfuck" "rot13" "abs" "is_prime" "quicksort" "pow_func_gen"]]# quit
```
### Serializing and deserializing forthscript values
```to_string``` converts any forthscript value to string
```
[]# [1 2 ["inner array" $a a a + Nil] "wow" that [[is]] cool] $arr
[]# arr to_string
["[1 2 [\"inner array\" $a a a + Nil] \"wow\" that [[is]] cool]"]#
```
```from_string``` parses forthscript values from string. It always returns an array, even if there is only one element in it
```
[]# "1 2 3" from_string
[[1 2 3]]#
```
### Error handling
Time to divide by zero!
```
[]# 1 0 /
Runtime error: Division by zero
```
This error and many others (array boundaries overflow, call stack overflow, lack of elements on the stack, user defined errors, type errors...) can be handeled using ```try``` native word.

 ```try``` takes two arguments: an array with code to execute and maximum number of arguments it is allowed to touch. After its execution, it pushes boolean on the stack, which will be True if execution was successful and False otherwise. If True is on the stack, the stack under this boolean consists of values left by subroutine. If the result is False, the stack is cleared in a way that only (stack size before call to try - 2 - arguments count) is left (2 here represents 2 arguments to try itself: code and arguments count)
```
[]# [$a a *] $sqr
[]# 5 3 sqr 1 try
[5 "Evaluation stack underflow" False]# 
```
In this case, ```try``` caught evaluation stack underflow exception: there is not enough operands for * to run. It can be easily seen that there is in fact two numbers on the stack when try executes: 5 and 3, but try doesn't allow code inside to access stack below given arguments count, so * can only access one number: 3.

```throw``` can be used to throw any user defined error. The argument to throw can have absolutely any type.

```
[]# [5 throw] 0 try
[5 False]# 
```
## Code examples
Checking if a given number is a prime number.
```
[ $n
    n 2 < [
        False return
    ] if
    [2 $i] [i i * n <=] [i 1 + =i] [
        n i % 0 == [
            False return
        ] if
    ] for
    True
] $is_prime
```
Getting absolute value of a number
```
[$n
  n 0 < [
    0 n - return
  ] [
    n return    
  ] if_else
] $abs
```
Quicksort for an array of integers
```
[ $hi $lo $A
	lo hi < [
		A lo hi partition! $p
		A lo p quicksort_rec!
		A p 1 + hi quicksort_rec!
	] if
	A
] $quicksort_rec

[ $hi $lo $A
	hi lo + 2 / $mid
	A mid peek $pivot
	lo $i
	hi $j
	[i j <=] [
		[A i peek pivot <] [i 1 + =i] while
		[A j peek pivot >] [j 1 - =j] while
		i j >= [
			break
		] if
		A i peek $tmp
		A i A j peek poke
		A j tmp poke
		i 1 + =i
		j 1 - =j
	] while
	j
] $partition

[ $A A 0 A len 1 - quicksort_rec!] $quicksort
```
Caesar cipher
```
[ $n $str
	"" $result
	[0 $i] [i str len <] [i 1 + =i] [
		str i peek $char
		char ord "a" ord - $ind
		ind n + 26 % =ind
		ind "a" ord + chr $char
		result char + =result
	] for
	result
] $rot_n
```
Function to calculate GCD of two numbers
```
[dup 0 != [swap over % gcd!] [drop] if_else] $gcd
```

Look at folder ```tests/cases``` or ```examples``` for more interesting examples.

## Roadmap

Current goals from highest to lowest prioriy

* NativeHandles. NativeHandle is a type exposed to native functions, that need some containers to store data. For instance, file descriptor, net socket or string builder object can be stored inside a NativeHandle, and some native functions will take this naitve handles as parametres to do things on them (read from file, load image from url, append string, ...).

* Floating/Bigint types. I am personally leaned to something floating types, as they don't need one more dependency (and I totally don't want to reinvent the wheel).

* Change string representation to ```const char32_t*; size_t``` from ```std::u32string```. The first option should be more performant as instantiating/memory overhead of mutability is avoided.

* Distributing as Static/Dynamic library. ForthScript is mostly suited to be used as scripting language, so there should be a way to easilly use ForthScript inside C/C++ application (C support is crucial, as it also allows to use interpreter from anything having valid C ffi).There will also be executable ForthScript interpreter, that will have way more rich standard library than embedded options.

* Loading Dynamic libraries in runtime. There is some work on this already in ```api``` branch, but it may be to early to do something like that stable

* Objects/tables. object/table is a hash map from string to forthscript values, that can be used to do some data & code encapsulation.

These ones are the lowest priority imaginable

* Package manager. For interpreter executable, there should be an option to install packages both with ForthScript and native code. There also should be some way to handle versions

* Making ForthScript compiled & interpreted (with optional jit) language. There are many performance issues in the current interpreter, so there is a plan to rewrite the whole thing both in C (for runtime that is absolutely neccessary like GC and variables lookup) and ForthScript (for the compiler and linker). This doesn't mean that anything from ForthScript is taken away, all reflective issues should be present all the way to bare metal.

## Dependencies

The project depends on https://github.com/arangodb/linenoise-ng for line editing in REPL and on https://github.com/sryze/glob for globbing. The files io/glob.cpp, io/glob.hpp, linenoise.h and src/linenoise/* are licensed by their respective licenses from original projects.