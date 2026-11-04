#include <iostream>
#include <string>

#include "CodeGen.h"

void CodeGen::emit(const std::string& line, int indent) {
    mText += std::string(indent*4, ' ') + line + "\n";
}

void CodeGen::emitData(const std::string& line) {
    mData += "    " + line + "\n";
}

int CodeGen::generate(const Node* root) {

    /* ------------------------- GENERATE CODE HERE ------------------------- */

    int returnVal = generate_(root);

    /* ---------------------------------------------------------------------- */

    // put the .data section in the output
    mOutput = ".include \"defs.inc\"\n\n.data\n";
    mOutput += mData;

    // write the .text section to the output
    mOutput += "\n.text\n.global _start\n\n_start:\n";
    mOutput += mText;

    // write the exit to the output
    mOutput += "\n    mov x0, #0\n    b EXIT\n";
    return returnVal;
}

int CodeGen::generate_(const Node* node) {
    if (!node) return 0;

    switch (node->type()) {
        // nonterminals
        case NodeType::BinaryOperation: {
            auto* binOpNode = static_cast<const BinaryOperation*>(node);
            genBinaryOp(binOpNode);

            break;
        }
        case NodeType::LangFunction: {
            auto* langFuncNode = static_cast<const LangFunction*>(node);
            genLangFunction(langFuncNode);
            break;
        }
        case NodeType::Negate: {
            auto* negateNode = static_cast<const Negate*>(node);
            genNegate(negateNode);
            break;
        }
    }

    return 0;
}


/* --------------------- TERMINALS --------------------- */

void CodeGen::genChild(const Node* node) {
    switch (node->type()) {
        case NodeType::Number: {
            auto* numNode = static_cast<const Number*>(node);
            genNumber(numNode);
            break;
        }
        case NodeType::String: {
            auto* strNode = static_cast<const String*>(node);
            genString(strNode);
            break;
        }
        case NodeType::Identifier: {
            auto* idNode = static_cast<const Identifier*>(node);
            genIdentifier(idNode);
            break;
        }
        case NodeType::Constant: {
            auto* constNode = static_cast<const Constant*>(node);
            genConstant(constNode);
            break;
        }
    }
}

void CodeGen::genNumber(const Number* numNode) {

}

void CodeGen::genIdentifier(const Identifier* idNode) {

}

void CodeGen::genConstant(const Constant* constNode) {

}

void CodeGen::genString(const String* strNode) {

    emitData("s" + std::to_string(mStringCount) + ": .ascii \"" + strNode->token.mValue + "\"");
    mStrings[strNode->token.mValue] = "s" + std::to_string(mStringCount);
}


/* -------------------- NONTERMINALS -------------------- */

void CodeGen::genBinaryOp(const BinaryOperation* binOpNode) {

}
void CodeGen::genNegate(const Negate* negateNode) {

}
void CodeGen::genLangFunction(const LangFunction* langFuncNode) {

    if (langFuncNode->functionToken.mValue == "print") {
        genChild(langFuncNode->arguments[0].get());

        if (langFuncNode->arguments[0]->type() == NodeType::String) {
            const auto* strToPrint = static_cast<const String*>(langFuncNode->arguments[0].get());

            emit("stp x29, x30, [sp, #-16]!", 1);
            emit("ldr x1, =" + mStrings[strToPrint->token.mValue], 1);
            emit("mov x2, #" + std::to_string(strToPrint->token.mValue.length()), 1);
            emit("bl WRITE", 1);
            emit("ldp x29, x30, [sp], #16", 1);
        }
    }
}
