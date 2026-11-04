#include <iostream>
#include <fstream>
#include <string>

#include "CodeGen.h"

void CodeGen::emit(const std::string& line, int indent) {
     mOutput += std::string(indent*4, ' ') + line + "\n";
}

int CodeGen::generate(const Node* root) {

    // put the .data section in the output

    mOutput = ".include \"defs.inc\"\n\n.data\n";

    for (const std::string& dataLine : mData) {
        mOutput += std::string(4, ' ') + dataLine + "\n";}

    // write the .text section to the output
    mOutput += "\n.text\n.global _start\n\n_start:\n";

    /* ------------------------- GENERATE CODE HERE ------------------------- */

    int returnVal = generate_(root);

    /* ---------------------------------------------------------------------- */

    // write the exit to the output
    mOutput += "\n    mov x0, #0\n    b EXIT\n";
    return returnVal;
}

int CodeGen::generate_(const Node* node) {
    if (!node) return 0;


    switch (node->type()) {
        case NodeType::Number:
            break;
        case NodeType::String:
            break;
        case NodeType::Identifier:
            break;
        case NodeType::Constant:
            break;
        case NodeType::BinaryOperation:
            break;
        case NodeType::LangFunction:
            break;
        case NodeType::Negate:
            break;
    }
}
