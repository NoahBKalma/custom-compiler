#include <algorithm>
#include <string>
#include <bit>

#include "CodeGen.h"

int CodeGen::generate(const Node* root) {

    /* ------------------------- GENERATE CODE HERE ------------------------- */

    ValueType returnVal = generateExpression(root);

    /* ---------------------------------------------------------------------- */

    // put the .data section in the output
    mOutput = ".include \"defs.inc\"\n\n.data\n    newline: .ascii \"\\n\"\n";
    mOutput += mData;

    // write the .text section to the output
    mOutput += "\n.text\n.global _start\n\n_start:\n";
    mOutput += mText;

    // write the exit to the output
    mOutput += "\n\n    ldr x1, =newline\n    mov x2, #1\n    bl WRITE\n\n    mov x0, #0\n    b EXIT\n";

    return returnVal == ValueType::Error ? 1 : 0;
}

ValueType CodeGen::generateExpression(const Node* node) {

    if (!node) return ValueType::None;

    switch (node->type()) {

        case NodeType::Program: {
            auto* program = static_cast<const Program*>(node);

            // iterate through every line in the program and generate an expression for that
            for (const auto& line : program->lines)
                if (generateExpression(line.get()) == ValueType::Error)
                    return ValueType::Error;

            return ValueType::None;
        }

        // terminals
        case NodeType::Number: {
            auto* numNode = static_cast<const Number*>(node);
            return genNumber(numNode);
        }
        case NodeType::String: {
            auto* strNode = static_cast<const String*>(node);
            return genString(strNode);
        }
        case NodeType::Identifier: {
            auto* idNode = static_cast<const Identifier*>(node);
            return genIdentifier(idNode);
        }
        case NodeType::Constant: {
            auto* constNode = static_cast<const Constant*>(node);
            return genConstant(constNode);
        }

        // nonterminals
        case NodeType::BinaryOperation: {
            auto* binOpNode = static_cast<const BinaryOperation*>(node);
            return genBinaryOp(binOpNode);
        }
        case NodeType::LangFunction: {
            auto* langFuncNode = static_cast<const LangFunction*>(node);
            return genLangFunction(langFuncNode);
        }
        case NodeType::Negate: {
            auto* negateNode = static_cast<const Negate*>(node);
            return genNegate(negateNode);
        }
        default: {
            mError = "Unhandled node type";
            return ValueType::Error;
        }
    }
}

/* --------------------- TERMINALS --------------------- */

ValueType CodeGen::genNumber(const Number* numNode) {

    if (contains(numNode->token.mValue, ".")) {
        // numNode is a float
        // convert float to the bitpattern

        float convertedFloat = std::stof(numNode->token.mValue);
        auto floatBits = std::bit_cast<uint32_t>(convertedFloat);
        emit("ldr w9, =" + std::to_string(floatBits));
        emit("fmov s0, w9");

        return ValueType::Float;
    } else {
        // numNode is an int
        emit("mov w0, #" + numNode->token.mValue);
        return ValueType::Int;
    }
}

ValueType CodeGen::genIdentifier(const Identifier* idNode) {
    mError = "Identifiers not implemented yet";
    return ValueType::Error;
}

ValueType CodeGen::genConstant(const Constant* constNode) {
    mError = "Constants not implemented yet";
    return ValueType::Error;
}

ValueType CodeGen::genString(const String* strNode) {

    emitData("s" + std::to_string(mStringCount) + ": .ascii \"" + strNode->token.mValue + "\"");
    mDataStrings[strNode->token.mValue] = "s" + std::to_string(mStringCount++);
    lastLabel = strNode->token.mValue;

    return ValueType::String;
}

/* -------------------- NONTERMINALS -------------------- */

ValueType CodeGen::genBinaryOp(const BinaryOperation* binOpNode) {

    ValueType binOpMode;

    ValueType leftReturnVal = generateExpression(binOpNode->left.get());
    if (leftReturnVal == ValueType::Error) return ValueType::Error;

    // handle identifier assignment
    if (binOpNode->token.mValue == "=") {
        if (leftReturnVal != ValueType::Identifier) {
            mError = "Left side of = must be an identifier.";
            return ValueType::Error;
        }

        // ALLOW FOR IDENTIFIER ASSIGNMENT
        mError = "Identifier assignment not implemented yet.";
        return ValueType::Error;
    }

    // concat strings with +
    if (leftReturnVal == ValueType::String && binOpNode->token.mValue == "+") {
        std::string strLeft = lastLabel;

        ValueType rightReturnVal = generateExpression(binOpNode->right.get());
        if (rightReturnVal == ValueType::Error) return ValueType::Error;
        if (rightReturnVal != ValueType::String) {
            mError = "Can only add a string to another string.";
            return ValueType::Error;
        }

        std::string strCombined = strLeft + lastLabel;
        emitData("s" + std::to_string(mStringCount) + ": .ascii \"" + strCombined + "\"");
        mDataStrings[strCombined] = "s" + std::to_string(mStringCount++);
        lastLabel = strCombined;

        return ValueType::String;
    }

    // save value so you don't overwrite w0
    if (leftReturnVal == ValueType::Int)
        emit("str w0, [sp, #-16]!");
    if (leftReturnVal == ValueType::Float)
        emit("str s0, [sp, #-16]!");

    ValueType rightReturnVal = generateExpression(binOpNode->right.get());
    if (rightReturnVal == ValueType::Error) return ValueType::Error;

    // move right to register 1 and load the left back into register 0
    if (leftReturnVal == ValueType::Int && rightReturnVal == ValueType::Int) {
        emit("mov w1, w0");
        emit("ldr w0, [sp], #16");

        // now w0 = left, w1 = right
        if (binOpNode->token.mValue == "+")
            emit("add w0, w0, w1");
        else if (binOpNode->token.mValue == "-")
            emit("sub w0, w0, w1");
        else if (binOpNode->token.mValue == "/")
            emit("sdiv w0, w0, w1");
        else if (binOpNode->token.mValue == "*")
            emit("mul w0, w0, w1");
        else if (binOpNode->token.mValue == "%") {
            emit("sdiv w2, w0, w1");
            emit("msub w0, w2, w1, w0");
        }
        else {
            mError = "Unknown operator: " + binOpNode->token.mValue;
            return ValueType::Error;
        }

        return ValueType::Int;

    }
    else if ((leftReturnVal == ValueType::Int || leftReturnVal == ValueType::Float) &&
            (rightReturnVal == ValueType::Int || rightReturnVal == ValueType::Float)) {
        if (rightReturnVal == ValueType::Float) {
            emit("fmov s1, s0");
        }
        else {
            emit("scvtf s1, w0");
        }

        // get s0 whether left is an int or a float
        if (leftReturnVal == ValueType::Float) {
            emit("ldr s0, [sp], #16");
        }
        else {
            emit("ldr w0, [sp], #16");
            emit ("scvtf s0, w0");
        }

        // now w0 = left, w1 = right
        if (binOpNode->token.mValue == "+")
            emit("fadd s0, s0, s1");
        else if (binOpNode->token.mValue == "-")
            emit("fsub s0, s0, s1");
        else if (binOpNode->token.mValue == "/")
            emit("fdiv s0, s0, s1");
        else if (binOpNode->token.mValue == "*")
            emit("fmul s0, s0, s1");
        else if (binOpNode->token.mValue == "%") {
            mError = "Cannot perform % on floats.";
            return ValueType::Error;
        }
        else {
            mError = "Unknown operation at: " + binOpNode->token.mValue;
            return ValueType::Error;
        }

        return ValueType::Float;
    }

    mError = "Unknown operation at operator: " + binOpNode->token.mValue;
    return ValueType::Error;
}

ValueType CodeGen::genNegate(const Negate* negateNode) {

    ValueType negateExpVal = generateExpression(negateNode->operand.get());
    if (negateExpVal == ValueType::Error) return ValueType::Error;

    if (negateExpVal == ValueType::Int) {
        emit("neg w0, w0");
        return ValueType::Int;
    }
    else if (negateExpVal == ValueType::Float) {
        emit("fneg s0, s0");
        return ValueType::Float;
    }
    else {
        mError = "Type not supported for negate.";
        return ValueType::Error;
    }

}

ValueType CodeGen::genLangFunction(const LangFunction* langFuncNode) {
    // ANY CHANGES TO PRINT COPY TO PRINTLN
    if (langFuncNode->functionToken.mValue == "print") {
        if (langFuncNode->arguments.empty()) return ValueType::None;

        const ValueType toPrint = generateExpression(langFuncNode->arguments[0].get());
        if (toPrint == ValueType::Error) return ValueType::Error;

        if (toPrint == ValueType::String) {
            std::string labelNum = mDataStrings[lastLabel];

            size_t len = lastLabel.length() - std::count(lastLabel.begin(), lastLabel.end(), '\\');

            emit("stp x29, x30, [sp, #-16]!");
            emit("ldr x1, =" + labelNum);

            emit("mov x2, #" + std::to_string(len));
            emit("bl WRITE");
            emit("ldp x29, x30, [sp], #16");
        }
        else if (toPrint == ValueType::Int) {
            emit("bl PRINT_INT");
        }
        else if (toPrint == ValueType::Float) {
            emit("bl PRINT_FLOAT");
        }
        else {
            mError = "Cannot print this type yet";
            return ValueType::Error;
        }
    }
    else if (langFuncNode->functionToken.mValue == "println") {
        if (langFuncNode->arguments.empty()) {
            printNewline();
            return ValueType::None;
        }

        const ValueType toPrint = generateExpression(langFuncNode->arguments[0].get());
        if (toPrint == ValueType::Error) return ValueType::Error;

        if (toPrint == ValueType::String) {
            std::string labelNum = mDataStrings[lastLabel];

            size_t len = lastLabel.length() - std::count(lastLabel.begin(), lastLabel.end(), '\\');

            emit("stp x29, x30, [sp, #-16]!");
            emit("ldr x1, =" + labelNum);

            emit("mov x2, #" + std::to_string(len));
            emit("bl WRITE");
            emit("ldp x29, x30, [sp], #16");
        }
        else if (toPrint == ValueType::Int) {
            emit("bl PRINT_INT");
        }
        else if (toPrint == ValueType::Float) {
            emit("bl PRINT_FLOAT");
        }
        else {
            mError = "Cannot print this type yet";
            return ValueType::Error;
        }

        // print a new line at the end
        printNewline();
    }

    return ValueType::None;
}