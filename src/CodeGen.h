#pragma once

#include <string>
#include <unordered_map>

#include "AST.h"

class CodeGen {
private:
    std::string mOutput;
    std::string mData;
    std::string mText;
    std::unordered_map<std::string, std::string> mDataStrings;
    std::string mError;
    std::unordered_map<std::string, int> mLocalVars; // name, count

    std::string lastLabel;
    size_t mStringCount;
    size_t mPrintNumCount;

    // append one instruction line to output
    void emit(const std::string& line, int indent = 1) { mText += std::string(indent*4, ' ') + line + "\n"; }

    // append one instruction line to data
    void emitData(const std::string& line) { mData += "    " + line + "\n"; }

    // generates code to print newline character
    void printNewline(int indent = 1) { emit("ldr x1, =newline\n    mov x2, #1\n    bl WRITE", indent); }

    // generate functions for each node type

    ValueType generateExpression(const Node*);

    // terminals
    ValueType genNumber(const Number*);
    ValueType genIdentifier(const Identifier*);
    ValueType genConstant(const Constant*);
    ValueType genString(const String*);

    // nonterminals
    ValueType genBinaryOp(const BinaryOperation*);
    ValueType genNegate(const Negate*);
    ValueType genLangFunction(const LangFunction*);

    static bool contains(const std::string& str, const std::string& subStr) { return str.find(subStr) != std::string::npos; }

public:

    CodeGen() :
        mStringCount { 0 } {}

    // walks the string and prints output
    int generate(const Node*);

    // The finished assembly: text section + data section
    std::string getOutput() const { return mOutput; }

    const std::string& getError() const { return mError; }
};