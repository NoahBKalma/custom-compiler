# custom-compiler

A small compiler written in C++20 that turns programs in a tiny custom language (`.nkg` files) into AArch64 assembly, then assembles and links them into a native Linux executable. It uses no external libraries and no libc: the generated program talks to the kernel directly through raw syscalls.

Right now the language can print string literals. The lexer and parser already understand arithmetic expressions, and code generation for them is the next thing being built.

## Example

`tests/code/print.nkg`:

```
print("Hello World\n");

print("this is on line 3\n");
```

Compile and run:

```
./compiler ../tests/code/print_string.nkg
./program
```

## Requirements

- AArch64 (ARM64) Linux, such as a Raspberry Pi. The generated assembly uses ARM64 syscall numbers, so it will not run on x86.
- A C++20 compiler (GCC or Clang)
- CMake 3.16 or newer
- GNU binutils (`as` and `ld`), which the compiler calls to produce the final executable

## Build

```
mkdir build
cd build
cmake ..
make
```

This produces a `compiler` executable inside `build/`.

## Usage

```
./compiler <input file>
```

Run it from your build directory. It writes `start.o`, `syscalls.o`, and the finished `program` into the current directory. Lexer, parser, and assembler errors are printed in red and the compiler exits with status 1.

## How it works

Source code moves through four stages:

1. **Lexer** (`src/Lexer.cpp`): splits the source text into tokens (numbers, identifiers, strings, operators, punctuation) and reports errors such as unterminated strings or two decimal points in one number.
2. **Parser** (`src/Parser.cpp`): a recursive descent parser that builds an abstract syntax tree (`src/AST.h`). Operator precedence is handled by splitting expressions into expression, term, and final levels.
3. **Code generator** (`src/CodeGen.cpp`): walks the tree and emits AArch64 assembly into `runtime/start.s`.
4. **Assemble and link** (`src/main.cpp`): runs `as` on `start.s` and `syscalls.s`, then links them with `ld` into `program`.

## Project layout

```
src/        compiler source (lexer, parser, AST, code generator, main)
runtime/    assembly that the generated program links against
            syscalls.s  WRITE and EXIT wrappers around Linux syscalls
            defs.inc    syscall numbers
            start.s     generated output (overwritten on every compile)
tests/      lexer and parser test inputs, plus sample .nkg programs in tests/code/
rules.md    language reference and implementation status
```

## Testing the lexer and parser

`tests/lexer-tests.txt` and `tests/parser-tests.txt` hold one test input per line. `main.cpp` contains `lexerTest()` and `parserTest()`, which print the tokens or a sideways view of the syntax tree for each line. They are currently commented out in `main()`. Uncomment the one you want and run the compiler from the `build/` directory, since the test paths are relative (`../tests/`).

## Language reference

See [rules.md](rules.md) for the grammar, the list of built-in functions, and what is implemented so far.
