SimpleBrainfuck
===============

SimpleBrainfuck is a set of tools for brainfuck programmers.

How to use
----------

The syntax is `sbf [file] <command> [<args>] <command> [<args>] ...`

If `file` isn't given, `sbf` read from stdin

For example: `sbf file.sbf compile strip -v --output out.bf run -v`

It is the same as `sbf file.sbf compile | sbf strip -v --output out.bf | sbf run -v`

### SimpleBrainfuck compiler: `sbf compile`

Compile SimpleBrainfuck to Brainfuck

Arguments:

* `-o [file]` or `--output [file]`: output the result to `file`. Default: none

### Brainfuck compressor: `sbf strip`

Strip comments, compress and optimize brainfuck

Arguments:

 * `-o [file]` or `--output [file]`: output the result to `file`. Default: none
 * `-v` or `--verbose`: switch in verbose mode, display statistics about the compression. Default: off

### Brainfuck fitter: `sbf fit`

Fit brainfuck program into an ascii-art image

Arguments:

 * `-o [file]` or `--output [file]`: output the result to `file`. Default: none
 * `-i [file]` or `--image [file]`: specify the image file
 * `-c [chars]` or `--chars [chars]`: specify the chars to replace by the brainfuck program. Default: `#`
 * `-v` or `--verbose`: switch in verbose mode, display statistics about the fitting. Default: off

### Brainfuck interpreter: `sbf run`

Optimized brainfuck interpreter (a bit faster than bff4, a bit slower than bffsree)

Arguments:

 * `-d [delimiter]` or `--delimiter [delimiter]`: specify an input delimiter, as for bff4 or self-interpreters. Default: none
 * `-p [number]` or `--positive-memory [number]`: specify the amount of positive memory
 * `-n [number]` or `--negative-memory [number]`: specify the amount of negative memory
 * `-g` or `--debug`: switch in debugger mode
 * `-v` or `--verbose`: switch in verbose mode, display statistics about the optimization

### Brainfuck debugger: `sbf run -g`

Help the debugging of brainfuck programs. Same arguments as `sbf run`, as the brainfuck debugger is itself an argument of `sbf run`.

Breakpoints are represented by the character `#` in your program

Commands:

 * `r` or `run`: run the program and stop at the next breakpoint
 * `hd [min:][max]` or `dump [min:][max]` or `hexdump [min:][max]`: make a hexdump from `min` to `max`. If `min` is omited, suppose 0.
 * `dd [min:][max]` or `dummydump [min:][max]`: make a simple dump from `min` to `max`. In `min` is omited, suppose 0.
 * `w [pos]` or `watch [pos]`: tell the debugger to break if the cell at position `pos` is modified

SimpleBrainfuck language reference
---------------------------------

### Variable management

 * `#variables(VariableWorld; variableName1; variableName2; ...)` define a variable world, and their position on the memory tape
 * `@VariableWorld@variableName;` tell to the compiler your are at `variableName` in `VariableWorld`
 * `=variableName;` move to `variableName` in the current world

Example:

    #variables(ProgDecFinal; continue; instr; zero1; zero2; input; counter)
    @ProgDecFinal@continue;
    =zero2; ++ #_ insert `>>> ++`
    =instr; - #_ insert `<< -`

### Macro support

 * `_macroName(arg1; arg2; arg3)` call `macroName` with `arg1`, `arg2`, `arg3`
 * `#macro(macroName; arg1; arg2; arg3;; code)` define a macro
 * `#cmacro(macroName; code)` define a macro written in C. You have `int argc` and `char** argv` to handle arguments. Call `push` to give the result

Example:

    #cmacro(add;
        int i;
        int nb = atoi(argv[0]);
        if(nb < 0) {
            for(i = 0; i < -nb; ++i) {
                push("-");
            }
        } else {
            for(i = 0; i < nb; ++i) {
                push("+");
            }
        }
    )

    #macro(moveTo; v1; v2;;
        =v1; [=v2; + =v1; -]
    )

    #macro(copy; v1; v2; temp;;
        =v1; [=v2; + =temp; + =v1; -]
        _moveTo(temp; v1)
    )

    #variables(VarWorld; var1; var2; var3)
    @VarWorld@var1;
    , _add(10) #_ Query a character and add 10 to it
    _copy(var1; var2; var3) #_ Copy var1 to var2, use var3 as temporary variable

### Repeating

Sometimes, you want to repeat an expression. You can do it in SimpleBrainfuck, like this:

    (+)10 #_ insert `++++++++++`
    (>+>-)5 #_ insert `>+>->+>->+>->+>->+>-`

### Comments and special characters

 * Use `#_` or `_#` to comment the rest of the line
 * Use `##` to insert a `#` (useful to use the debugger), and `__` to insert a `_`

FAQ
---
**The SimpleBrainfuck language is close to brainfuck, which is complicated. Why do you call it "simple"?**

You are right it should be "Simple**r**Brainfuck". But "Simple" is a joke, because almost every software / protocol with "simple" in their name is complicated, like SBT (Simple Build Tool) or SNMP (Simple Network Management Protocol).

**I get segfaults in the SimpleBrainfuck compiler!**

Well, the error handling in the compiler is pretty bad, it's on the todo-list to improve it.

**There are much simpler languages that compile to brainfuck, like bfcomp, ebf, and others. The SimpleBrainfuck language is not the simplest language that compiles in brainfuck.**

The aim of the SimpleBrainfuck language isn't to provide a high level language, but to simplify brainfuck programming, and eliminate redundancies.

