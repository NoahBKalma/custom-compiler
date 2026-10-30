#include <iostream>

// Runs a shell command, prints an error and returns false if it fails
static bool run(const char* cmd) {
    if (std::system(cmd) != 0) {
        std::cerr << "Error: Command Failed: " << cmd << "\n";
        return false;
    }
    return true;
}

int main(const int argc, const char* argv[]) {
    // 2. Read the source file into a string
    // 3. Lexer: source text -> tokens
    // 4. Parser: tokens -> AST
    // 5. Codegen: AST -> assembly text
    // 6. Write the assembly to an output file
    // 7. Assemble and link with the runtime (using RUNTIME_DIR)

    if (argc < 2) { // no missing file to compile argument
        std::cerr << "Missing argument.\nUsage: compiler.exe " << "<input file>\n";
        return 1;
    }

    if (!run("as -I " RUNTIME_DIR " " RUNTIME_DIR "/start.s -o start.o")) return 1;
    if (!run("as -I " RUNTIME_DIR " " RUNTIME_DIR "/syscalls.s -o syscalls.o")) return 1;
    if (!run("ld start.o syscalls.o -o prog")) return 1;

    return 0;
}