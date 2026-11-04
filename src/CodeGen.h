#pragma once

#include <string>
#include <vector>

#include "AST.h"

class CodeGen {
private:
    std::string mOutput;
    std::vector<std::string> mData;
    std::vector<std::string> mErrors;

    size_t mStringCount;

    // append one instruction line to output
    void emit(const std::string&, int);

    // store text, return its label
    std::string newStringData(const std::string&);

    // generate functions for each node type

    // terminals
    void genNumber(const Number*);
    void genIdentifier(const Identifier*);
    void genConstant(const Constant*);
    void genString(const String*);

    // nonterminals
    void genBinary(const BinaryOperation*);
    void genNegate(const Negate*);
    void genFunction(const LangFunction*);

    int generate_(const Node*);

public:

    CodeGen() :
        mStringCount { 0 } {}

    // walks the string and prints output
    int generate(const Node*);

    // The finished assembly: text section + data section
    std::string getOutput() const { return mOutput; }

    const std::vector<std::string>& getErrors() const { return mErrors; }
};