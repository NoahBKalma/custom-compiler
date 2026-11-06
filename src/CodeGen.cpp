#include <algorithm>
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

    int returnVal = generateNonTerminal(root);

    /* ---------------------------------------------------------------------- */

    // put the .data section in the output
    mOutput = ".include \"defs.inc\"\n\n.data\n    newline: .ascii \"\\n\"\n";
    mOutput += mData;

    // write the .text section to the output
    mOutput += "\n.text\n.global _start\n\n_start:\n";
    mOutput += mText;

    // write the exit to the output
    mOutput += "\n\n    ldr x1, =newline\nmov x2, #1\nbl WRITE\n\n    mov x0, #0\n    b EXIT\n";
    return returnVal;
}

int CodeGen::generateNonTerminal(const Node* node) {
    if (!node) return 0;

    switch (node->type()) {

        case NodeType::Program: {
            auto* program = static_cast<const Program*>(node);
            for (const auto& line : program->lines)
                generateNonTerminal(line.get());
            break;
        }

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

int CodeGen::genTerminal(const Node* node) {
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

    return 0;
}

int CodeGen::genNumber(const Number* numNode) {

    if (contains(numNode->token.mValue, ".")) {
        // numNode is a floa
        emit("mov s0, #" + numNode->token.mValue, 1);
    } else {
        // numNode is an int
        emit("mov w0, #" + numNode->token.mValue, 1);
    }

    return 0;
}

int CodeGen::genIdentifier(const Identifier* idNode) {

    return 0;
}

int CodeGen::genConstant(const Constant* constNode) {

    return 0;
}

int CodeGen::genString(const String* strNode) {

    emitData("s" + std::to_string(mStringCount) + ": .ascii \"" + strNode->token.mValue + "\"");
    mDataStrings[strNode->token.mValue] = "s" + std::to_string(mStringCount++);

    return 0;
}


/* -------------------- NONTERMINALS -------------------- */

int CodeGen::genBinaryOp(const BinaryOperation* binOpNode) {

    return 0;
}

int CodeGen::genNegate(const Negate* negateNode) {

    return 0;
}

int CodeGen::genLangFunction(const LangFunction* langFuncNode) {
    // ANY CHANGES TO PRINT COPY TO PRINTLN
    if (langFuncNode->functionToken.mValue == "print") {
        if (langFuncNode->arguments.empty()) return 0;

        genTerminal(langFuncNode->arguments[0].get());

        switch (langFuncNode->arguments[0]->type()) {
            case (NodeType::String): {
                const auto* stringNode = static_cast<const String*>(langFuncNode->arguments[0].get());
                const std::string& strToPrint = stringNode->token.mValue;

                size_t len = strToPrint.length() - std::count(strToPrint.begin(), strToPrint.end(), '\\');

                emit("stp x29, x30, [sp, #-16]!", 1);
                emit("ldr x1, =" + mDataStrings[strToPrint], 1);

                emit("mov x2, #" + std::to_string(len), 1);
                emit("bl WRITE", 1);
                emit("ldp x29, x30, [sp], #16", 1);

                break;
            }
            case (NodeType::Number): {
                const auto* numNode = static_cast<const String*>(langFuncNode->arguments[0].get());
                const std::string& strToPrint = numNode->token.mValue;

                // add number you're printing to data section
                emitData("s" + std::to_string(mStringCount) + ": .ascii \"" + strToPrint + "\"");
                mDataStrings[strToPrint] = "s" + std::to_string(mStringCount++);

                // assign the length of the buffer based on the string
                size_t len = strToPrint.length() - std::count(strToPrint.begin(), strToPrint.end(), '\\');

                // write assembly for print
                emit("stp x29, x30, [sp, #-16]!", 1);
                emit("ldr x1, =" + mDataStrings[strToPrint], 1);

                emit("mov x2, #" + std::to_string(len), 1);
                emit("bl WRITE", 1);
                emit("ldp x29, x30, [sp], #16", 1);

                break;
            }
            case (NodeType::BinaryOperation): {
                break;
            }
        }
    }
    else if (langFuncNode->functionToken.mValue == "println") {
        if (langFuncNode->arguments.empty()) {
            printNewline(1);
            return 0;
        }

        genTerminal(langFuncNode->arguments[0].get());

        switch (langFuncNode->arguments[0]->type()) {
            case (NodeType::String): {
                const auto* stringNode = static_cast<const String*>(langFuncNode->arguments[0].get());
                const std::string& strToPrint = stringNode->token.mValue;

                size_t len = strToPrint.length() - std::count(strToPrint.begin(), strToPrint.end(), '\\');

                emit("stp x29, x30, [sp, #-16]!", 1);
                emit("ldr x1, =" + mDataStrings[strToPrint], 1);

                emit("mov x2, #" + std::to_string(len), 1);
                emit("bl WRITE", 1);
                emit("ldp x29, x30, [sp], #16", 1);

                break;
            }
            case (NodeType::Number): {
                const auto* numNode = static_cast<const String*>(langFuncNode->arguments[0].get());
                const std::string& strToPrint = numNode->token.mValue;

                // add number you're printing to data section
                emitData("s" + std::to_string(mStringCount) + ": .ascii \"" + strToPrint + "\"");
                mDataStrings[strToPrint] = "s" + std::to_string(mStringCount++);

                // assign the length of the buffer based on the string
                size_t len = strToPrint.length() - std::count(strToPrint.begin(), strToPrint.end(), '\\');

                // write assembly for print
                emit("stp x29, x30, [sp, #-16]!", 1);
                emit("ldr x1, =" + mDataStrings[strToPrint], 1);

                emit("mov x2, #" + std::to_string(len), 1);
                emit("bl WRITE", 1);
                emit("ldp x29, x30, [sp], #16", 1);

                break;
            }
            case (NodeType::BinaryOperation): {
                break;
            }
        }
        // print a new line at the end
        printNewline(1);
    }

    return 0;
}