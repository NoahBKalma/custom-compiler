#pragma once

#include <string>
#include <vector>

#include "AST.h"

class CodeGen {
private:
    std::string mOutput;
    std::string mData;
    std::string mText;
    std::unordered_map<std::string, std::string> mDataStrings;
    std::vector<std::string> mErrors;

    size_t mStringCount;
    size_t mPrintNumCount;

    // append one instruction line to output
    void emit(const std::string&, int);

    // append one instruction line to data
    void emitData(const std::string&);

    // store text, return its label
    std::string newStringData(const std::string&);

    // generates code to print newline character
    void printNewline(int indent) { emit("ldr x1, =newline\n    mov x2, #1\n    bl WRITE", 1); }

    // generate functions for each node type

    // terminals
    int genTerminal(const Node*);

    int genNumber(const Number*);
    int genIdentifier(const Identifier*);
    int genConstant(const Constant*);
    int genString(const String*);

    // nonterminals
    int generateNonTerminal(const Node*);

    int genBinaryOp(const BinaryOperation*);
    int genNegate(const Negate*);
    int genLangFunction(const LangFunction*);

    static bool contains(const std::string& str, const std::string& subStr) { return str.find(subStr) != std::string::npos; }

public:

    CodeGen() :
        mStringCount { 0 } {}

    // walks the string and prints output
    int generate(const Node*);

    // The finished assembly: text section + data section
    std::string getOutput() const { return mOutput; }

    const std::vector<std::string>& getErrors() const { return mErrors; }
};