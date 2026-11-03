#include <iostream>

#include "Parser.h"

const Token& Parser::peekToken() const {
    return mLexedTokens[mCursor];
}

TokenType Parser::peekTokenType() const {
    return mLexedTokens[mCursor].mType;
}

const Token& Parser::nextToken() {
    if (mFinished)
        return mLexedTokens[mCursor];

    if (mLexedTokens[++mCursor].mType == TokenType::EndOfFile)
        mFinished = true;

    return mLexedTokens[mCursor - 1];
}

int Parser::parse() {

    // parses a single expression (allows for single line programs right now)
    mResultTree = parseExpression();

    if (!mResultTree) return 1;

    // make sure whole file is parsed
    if (peekTokenType() != TokenType::EndOfFile) {
        mErrorMessage = "Unexpected token " + peekToken().mValue;
        return 1;
    }

    return 0;
}

std::unique_ptr<Node> Parser::parseExpression() {
    std::unique_ptr<Node> left = parseTerm();

    if (!left) return nullptr;

    while (peekTokenType() == TokenType::Operator &&
            (peekToken().mValue == "+" || peekToken().mValue == "-")) {
        Token op = nextToken();
        std::unique_ptr<Node> right = parseTerm();
        if (!right) return nullptr;
        left = std::make_unique<BinaryOperation>(op, std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<Node> Parser::parseTerm() {
    std::unique_ptr<Node> left = parseFinal();

    if (!left) return nullptr;

    while (peekTokenType() == TokenType::Operator &&
            (peekToken().mValue == "*" || peekToken().mValue == "/")) {
        Token op = nextToken();
        std::unique_ptr<Node> right = parseFinal();
        if (!right) return nullptr;
        left = std::make_unique<BinaryOperation>(op, std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<Node> Parser::parseFinal() {
    if (peekTokenType() == TokenType::Number) {
        return std::make_unique<Number>(nextToken());
    }
    if (peekTokenType() == TokenType::String) {
        return std::make_unique<String>(nextToken());
    }
    if (peekTokenType() == TokenType::Constant) {
        return std::make_unique<Constant>(nextToken());
    }
    if (peekTokenType() == TokenType::Identifier) {
        return std::make_unique<Identifier>(nextToken());
    }
    if (peekTokenType() == TokenType::ParenOpen) {

        // Skip past opening delimiter then parse expression within
        nextToken();
        std::unique_ptr<Node> result = parseExpression();
        if (result == nullptr) return nullptr;
        if (peekTokenType() == TokenType::ParenClose) {
            nextToken();
            return result;
        }

        if (peekTokenType() == TokenType::EndOfFile)
            mErrorMessage = "Unclosed parenthesis. Reached end of file.";
        else
            mErrorMessage = "Expected closing parenthesis. Got " + peekToken().mValue;
        return nullptr;

    }
    if (peekTokenType() == TokenType::Operator && peekToken().mValue == "-") {

        Token negate = nextToken();
        std::unique_ptr<Node> operand = parseFinal();
        if (operand == nullptr) return nullptr;
        return std::make_unique<Negate>(negate, std::move(operand));

    }
    if (peekTokenType() == TokenType::Function) {
        Token functionToken = nextToken();
        std::unique_ptr<Function> function = std::make_unique<Function>(functionToken);
        do {
            if (peekTokenType() == TokenType::ParenOpen ||
                (peekToken().mValue == "," && peekTokenType() == TokenType::Comma)) {

                nextToken(); // skip opening parenthesis or comma while in loop
                } else {
                    if (peekTokenType() == TokenType::EndOfFile)
                        mErrorMessage = "Reached end of file before function call closed.";
                    else
                        mErrorMessage = "Unexpected token " + peekToken().mValue + " while trying to parse function call.";
                    return nullptr;
                }

            std::unique_ptr<Node> argument = parseExpression();
            if (!argument) return nullptr;

            function->arguments.push_back(std::move(argument));
        } while (peekToken().mValue == "," && peekTokenType() == TokenType::Comma);
        if (peekToken().mType != TokenType::ParenClose) {
            if (peekTokenType() == TokenType::EndOfFile)
                mErrorMessage = "Reached end of file before function call closed.";
            else
                mErrorMessage = "Unexpected token " + peekToken().mValue + " while trying to parse function call.";
            return nullptr;
        }

        nextToken();
        return function;
    }

    if (peekTokenType() == TokenType::EndOfFile)
        mErrorMessage = "Reached end of file before expression parsed.";
    else
        mErrorMessage = "Unexpected token " + peekToken().mValue;
    return nullptr;
}