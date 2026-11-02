#include <iostream>
#include <fstream>
#include <string>

#include "Tokens.h"
#include "Lexer.h"


static std::string redText(const std::string& message) {
    return "\033[31m" + message + "\033[0m";
}

// Runs a shell command, prints an error and returns false if it fails
static bool runCmd(const char* cmd) {
    if (std::system(cmd) != 0) {
        std::cerr << redText("Error: Command Failed: ") << cmd << "\n";
        return false;
    }
    return true;
}

static int lexerTest() {
    // Open for reading
    std::ifstream lexerTest("../tests/lexer-tests.txt");

    // Check if file opened successfully
    if (!lexerTest.is_open()) {
        std::cerr << "Error: Could not open lexer test file." << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(lexerTest, line)) {
        auto tokenized = Lexer::tokenize(line);

        std::vector<Token> lineTokenVec = tokenized.first;
        for (const Token& token : lineTokenVec) {
            std::cout << token.mValue << " (" << typeName(token.mType) << "), ";
        }

        for (std::string& error : tokenized.second) {
            std::cerr << "\n" << redText(error);
        }

        std::cout << std::endl << std::endl;
    }

    // Close the file
    lexerTest.close();
    return 0;
}

int main(const int argc, const char* argv[]) {

    if (argc < 2) { // no missing file to compile argument
        std::cerr << redText("Missing argument.\nUsage: compiler.exe <input file>\n");
        return 1;
    }

    int lexerTestResult = lexerTest();
    if (lexerTestResult == 1) {
        std::cerr << redText("Error in lexerTest") << std::endl;
    }


    if (!runCmd("as -I " RUNTIME_DIR " " RUNTIME_DIR "/start.s -o start.o")) return 1;
    if (!runCmd("as -I " RUNTIME_DIR " " RUNTIME_DIR "/syscalls.s -o syscalls.o")) return 1;
    if (!runCmd("ld start.o syscalls.o -o prog")) return 1;

    return 0;
}